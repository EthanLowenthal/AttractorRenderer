#pragma once
#include <OpenGL/gl3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <vector>
#include <array>
#include <thread>

struct Palette {
    std::vector<std::pair<float, std::array<float, 3>>> colors;
    void addColor(float position, float r, float g, float b) {
        colors.push_back({ position, { r, g, b } });
        sort();
    }
    void sort() {
        std::sort(colors.begin(), colors.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });
    }
    std::array<float, 3> getColor(float position) const {
        if (colors.empty()) return { 0.0f, 0.0f, 0.0f };
        if (position <= colors.front().first) return colors.front().second;
        if (position >= colors.back().first) return colors.back().second;

        for (size_t i = 0; i < colors.size() - 1; i++) {
            if (position >= colors[i].first && position <= colors[i + 1].first) {
                float t = (position - colors[i].first) / (colors[i + 1].first - colors[i].first);
                std::array<float, 3> result;
                for (int j = 0; j < 3; j++) {
                    result[j] = colors[i].second[j] * (1.0f - t) + colors[i + 1].second[j] * t;
                }
                return result;
            }
        }
        return colors.back().second;
    }
};

struct ImageConfig {
    int width = 500, height = 500;
    float bg_color[3] = { 0.0f, 0.0f, 0.0f };
    float scale = 1.0f;
    Palette palette;
};

struct CliffordConfig: public ImageConfig  {
    float a=-1.8f, b=-2.0f, c=-0.5f, d=-0.9f;
    int num_points=100, num_iterations=100000;
};

void iterate_clifford(const CliffordConfig& config, int* hits, std::atomic<float>& progress) {
    for (int p=0; p < config.num_points; p++) {
        float x = ((float)rand() / RAND_MAX) * 6.0f - 3.0f;
        float y = ((float)rand() / RAND_MAX) * 6.0f - 3.0f;

        float increment = 1.0f / config.num_points / std::thread::hardware_concurrency();
        float old_progress = progress.load();
        while (!progress.compare_exchange_weak(old_progress, old_progress + increment));
        
        for (int i = 0; i < 100; i++) {
            float xnew = sin(config.a * y) + config.c * cos(config.a * x);
            float ynew = sin(config.b * x) + config.d * cos(config.b * y);
            x = xnew;
            y = ynew;
        }
        
        for (int i = 0; i < config.num_iterations; i++) {
            float xnew = sin(config.a * y) + config.c * cos(config.a * x);
            float ynew = sin(config.b * x) + config.d * cos(config.b * y);
            x = xnew;
            y = ynew;
            
            int px = (int)((((x + 3.0f) / 6.0f - 0.5f) * config.scale + 0.5f) * config.width);
            int py = (int)((((y + 3.0f) / 6.0f - 0.5f) * config.scale + 0.5f) * config.height);
            
            if (px >= 0 && px < config.width && py >= 0 && py < config.height) {
                hits[py * config.width + px]++;
            }
        }
    }
}

void generate_clifford_image(const CliffordConfig& config, std::atomic<float>& progress) {
    int* hits = new int[config.width * config.height]();

    int n_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (int t = 0; t < n_threads; t++) {
        threads.emplace_back([&config, hits, &progress, t, n_threads]() {
            int points_per_thread = config.num_points / n_threads;
            CliffordConfig thread_config = config;
            thread_config.num_points = points_per_thread;
            iterate_clifford(thread_config, hits, progress);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    int maxHits = 1;
    for (int i = 0; i < config.width * config.height; i++) {
        if (hits[i] > maxHits) maxHits = hits[i];
    }
    
    unsigned char* image = new unsigned char[config.width * config.height * 3];
    for (int i = 0; i < config.width * config.height; i++) {
        float brightness = 0.0f;
        if (hits[i] > 0) {
            brightness = log(1.0f + hits[i]) / log(1.0f + maxHits);
        }
        
        std::array<float, 3> color = config.palette.getColor(brightness);
        
        unsigned char r = (unsigned char)(config.bg_color[0] * 255 * (1.0f - brightness) + color[0] * 255 * brightness);
        unsigned char g = (unsigned char)(config.bg_color[1] * 255 * (1.0f - brightness) + color[1] * 255 * brightness);
        unsigned char b = (unsigned char)(config.bg_color[2] * 255 * (1.0f - brightness) + color[2] * 255 * brightness);
        
        image[i * 3 + 0] = r;
        image[i * 3 + 1] = g;
        image[i * 3 + 2] = b;
    }
    
    stbi_write_png("clifford.png", config.width, config.height, 3, image, config.width * 3);
    
    delete[] image;
    delete[] hits;

    progress = 0.0f;
}