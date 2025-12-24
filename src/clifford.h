#pragma once


#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image.h>

#include <vector>
#include <array>
#include <thread>
#include <algorithm>
#include <atomic>

#include "image.h"
#include "palette.h"



struct ImageConfig {
    int width = 500, height = 500;
    float bg_color[3] = { 0.0f, 0.0f, 0.0f };
    float scale = 1.0f;
    Palette palette;
};


struct CliffordAttractor: public ImageConfig {
    float a=-1.8f, b=-2.0f, c=-0.5f, d=-0.9f;
    int num_points = 1000;
    int num_iterations = 1000;
    int points_per_thread = 0;
    int maxHits = 1;
    bool dirty = false;

    std::vector<int> hits {};
    std::vector<unsigned char> image {};

    GLuint image_texture = 0;
    int image_width = 0;
    int image_height = 0;


    void iterate(std::atomic<float>& progress) {
        for (int p=0; p < num_points; p++) {
            float x = ((float)rand() / RAND_MAX) * 6.0f - 3.0f;
            float y = ((float)rand() / RAND_MAX) * 6.0f - 3.0f;

            float increment = 1.0f / num_points / std::thread::hardware_concurrency();
            float old_progress = progress.load();
            while (!progress.compare_exchange_weak(old_progress, old_progress + increment));

            for (int i = 0; i < 100; i++) {
                float xnew = sin(a * y) + c * cos(a * x);
                float ynew = sin(b * x) + d * cos(b * y);
                x = xnew;
                y = ynew;
            }

            for (int i = 0; i < num_iterations; i++) {
                float xnew = sin(a * y) + c * cos(a * x);
                float ynew = sin(b * x) + d * cos(b * y);
                x = xnew;
                y = ynew;

                int px = (int)((((x + 3.0f) / 6.0f - 0.5f) * scale + 0.5f) * width);
                int py = (int)((((y + 3.0f) / 6.0f - 0.5f) * scale + 0.5f) * height);

                if (px >= 0 && px < width && py >= 0 && py < height) {
                    hits[py * width + px]++;
                }
            }
        }
    }

    void generate_hits(std::atomic<float>& progress) {
        hits.clear();
        hits.resize(width * height);
        hits.shrink_to_fit();

        image_width = width;
        image_height = height;

        int n_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        points_per_thread = num_points / n_threads;

        for (int t = 0; t < n_threads; t++) {
            threads.emplace_back([&]() {
                this->iterate(progress);
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        maxHits = 1;
        for (int i = 0; i < width * height; i++) {
            if (hits[i] > maxHits) maxHits = hits[i];
        }

        progress = 0.0f;
        dirty = true;
    }

    void generate_image() {
        image.clear();
        image.resize(image_width * image_height * 3);
        image.shrink_to_fit();

        for (int i = 0; i < image_width * image_height; i++) {
            float brightness = 0.0f;
            if (hits[i] > 0) {
                brightness = log(1.0f + hits[i]) / log(1.0f + maxHits);
            }

            if (brightness > 1.0f) brightness = 1.0f;

            ImVec4 color = palette.getColor(brightness);

            unsigned char r = (unsigned char)(bg_color[0] * 255 * (1.0f - brightness) + color.x * 255 * brightness);
            unsigned char g = (unsigned char)(bg_color[1] * 255 * (1.0f - brightness) + color.y * 255 * brightness);
            unsigned char b = (unsigned char)(bg_color[2] * 255 * (1.0f - brightness) + color.z * 255 * brightness);

            image[i * 3 + 0] = r;
            image[i * 3 + 1] = g;
            image[i * 3 + 2] = b;
        }

        dirty = false;

        load_image_to_texture(image.data(), image_width, image_height, &image_texture);
    }
};

