//
// Created by ethan on 7/25/23.
//

#ifndef ATTRACTORRENDERER_ATTRACTOR_H
#define ATTRACTORRENDERER_ATTRACTOR_H

#include <cmath>
#include <string.h>
#include "lodepng.h"
#include "vector.h"
#include "Palette.h"
#include "Buffer.h"

vec2<float> clifford(vec2<float> p, float a, float b, float c, float d) {
    return {
        std::sin(a * p.y) + c * std::cos(a * p.x),
        std::sin(b * p.x) + d * std::cos(b * p.y)
    };
}

struct Attractor {

    int supersample { 2 };
    vec2<int> size { 500, 500 };
    Buffer<unsigned> map { size.x, size.y };
    Buffer<unsigned char> image { size.x * 4, size.y };

    Buffer<float> histogram { 200, 1 };
    vec2<vec2<float>> bounds {
            {-2,2},
            {-2, 2}
    };

    vec2<vec2<float>> rendered_bounds = bounds;

    int iterations { 1000000 };

    int max_exposure { 0 };

    float gamma {1.5};
    float exposure {0.8};
    float base {3};

    Palette palette {
            {0.5, 0.5, 0.5},
            {0.5, 0.5, 0.5},
            {1.0, 1.0, 1.0},
            {0.0, 0.1, 0.2}
    };

    float a = -1.4;
    float b = 1.6;
    float c = 1.0;
    float d = 0.7;

    void iterate(int &progress) {
        vec2<float> p { 0, 0 };

        map.resize(size.x * supersample, size.y * supersample);
        map.fill(0);

        vec2<float> scale {
                (map.width() - 1) / (bounds.x[1] - bounds.x[0]),
                (map.height() - 1) / (bounds.y[1] - bounds.y[0]),
        };


        max_exposure = 0;

        for (int i=0;i<iterations;i++) {
            progress++;
//            if (log) { if (i % reset == 0) std::cout << "." << std::flush; }

            p = clifford(p, a, b, c, d);

            vec2<int> plot = {
                    static_cast<int>((floor((-p.x - bounds.x[0]) * scale.x))),
                    static_cast<int>((floor((-p.y - bounds.y[0]) * scale.y)))
            };

            plot = {
                    std::max(0, std::min(plot.x, map.width()-1)),
                    std::max(0, std::min(plot.y, map.height()-1))
            };


            unsigned val = map.get(plot.x, plot.y) + 1;

            if (val > max_exposure) max_exposure = val;

            map.set(plot.x, plot.y, val);
        }

        develop();

        rendered_bounds = bounds;
    }

    void develop() {
        histogram.fill(0);
        image.resize(size.x * 4, size.y);
        for(int x=0;x<size.x;x++) {
            for(int y=0;y<size.y;y++) {
                float val = 0;
                for (int ix=0;ix<supersample;ix++) {
                    for (int iy=0;iy<supersample;iy++) {
                        val += map.get(supersample * x + ix, supersample * y + iy);
                    }
                }
                val /= static_cast<float>(max_exposure) * supersample * supersample;

                val *= exposure;
                val = pow(val, 1.0 / gamma);
//                val = std::log(val)/log(base) + 1;
                val = std::max(0.0f, std::min(1.0f, val));

                if (val > 0)
                    histogram.set((histogram.width()-1) * val, 0, histogram.get((histogram.width()-1) * val, 0) + 1);

                auto color = get_color(val, palette);

                image.set(4 * x + 0, y, color.x * 255.0);
                image.set(4 * x + 1, y, color.y * 255.0);
                image.set(4 * x + 2, y, color.z * 255.0);
                image.set(4 * x + 3, y, 255.0);
            }
        }
    }

    void save(const std::string& filename) {

        lodepng::State state;
        LodePNGInfo& info = state.info_png;

//        lodepng_add_text(&info, "has_attractor_data", "true");
        lodepng_add_text(&info, "a", std::to_string(a).c_str());
        lodepng_add_text(&info, "b", std::to_string(b).c_str());
        lodepng_add_text(&info, "c", std::to_string(c).c_str());
        lodepng_add_text(&info, "d", std::to_string(d).c_str());
        lodepng_add_text(&info, "attractor", "clifford");
        lodepng_add_text(&info, "left_x", std::to_string(bounds.x[0]).c_str());
        lodepng_add_text(&info, "right_x", std::to_string(bounds.x[1]).c_str());
        lodepng_add_text(&info, "left_y", std::to_string(bounds.y[0]).c_str());
        lodepng_add_text(&info, "right_y", std::to_string(bounds.y[1]).c_str());
        lodepng_add_text(&info, "gamma", std::to_string(gamma).c_str());
        lodepng_add_text(&info, "exposure", std::to_string(exposure).c_str());

        lodepng_add_text(&info, "pal_a_x", std::to_string(palette.a.x).c_str());
        lodepng_add_text(&info, "pal_a_y", std::to_string(palette.a.y).c_str());
        lodepng_add_text(&info, "pal_a_z", std::to_string(palette.a.z).c_str());

        lodepng_add_text(&info, "pal_b_x", std::to_string(palette.b.x).c_str());
        lodepng_add_text(&info, "pal_b_y", std::to_string(palette.b.y).c_str());
        lodepng_add_text(&info, "pal_b_z", std::to_string(palette.b.z).c_str());

        lodepng_add_text(&info, "pal_c_x", std::to_string(palette.c.x).c_str());
        lodepng_add_text(&info, "pal_c_y", std::to_string(palette.c.y).c_str());
        lodepng_add_text(&info, "pal_c_z", std::to_string(palette.c.z).c_str());

        lodepng_add_text(&info, "pal_d_x", std::to_string(palette.d.x).c_str());
        lodepng_add_text(&info, "pal_d_y", std::to_string(palette.d.y).c_str());
        lodepng_add_text(&info, "pal_d_z", std::to_string(palette.d.z).c_str());


        std::vector<unsigned char> png;

        unsigned error = lodepng::encode(png, &image.array[0], size.x, size.y, state);
        lodepng::save_file(png, filename + ".png");
        //if there's an error, display it
        if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
    }
    void load(std::string filename) {
        std::vector<unsigned char> png;
//        std::vector<unsigned char> image; //the raw pixels
        unsigned width, height;
        lodepng::State state; //optionally customize this one

        unsigned error = lodepng::load_file(png, filename); //load the image file with given filename
        if(!error) error = lodepng::decode(image.array, width, height, state, png);
        //if there's an error, display it
        if(error) std::cout << "decoder error " << error << ": "<< lodepng_error_text(error) << std::endl;

        size.x = width;
        size.y = height;

        LodePNGInfo& info = state.info_png;

        for (int i=0;i<info.text_num;i++) {
            std::cout << info.text_keys[i] << ": " << info.text_strings[i] << std::endl;
            if(!strcmp(info.text_keys[i], "attractor")) continue;

            float val = std::stof(info.text_strings[i]);
            if(!strcmp(info.text_keys[i], "a")) a = val;
            else if(!strcmp(info.text_keys[i], "b")) b = val;
            else if(!strcmp(info.text_keys[i], "c")) c = val;
            else if(!strcmp(info.text_keys[i], "d")) d = val;
            else if(!strcmp(info.text_keys[i], "gamma")) gamma = val;
            else if(!strcmp(info.text_keys[i], "exposure")) exposure = val;
            else if(!strcmp(info.text_keys[i], "left_x")) bounds.x[0] = val;
            else if(!strcmp(info.text_keys[i], "right_x")) bounds.x[1] = val;
            else if(!strcmp(info.text_keys[i], "left_y")) bounds.y[0] = val;
            else if(!strcmp(info.text_keys[i], "right_y")) bounds.y[1] = val;
            else if(!strcmp(info.text_keys[i], "pal_a_x")) palette.a.x = val;
            else if(!strcmp(info.text_keys[i], "pal_a_y")) palette.a.y = val;
            else if(!strcmp(info.text_keys[i], "pal_a_z")) palette.a.z = val;
            else if(!strcmp(info.text_keys[i], "pal_b_x")) palette.b.x = val;
            else if(!strcmp(info.text_keys[i], "pal_b_y")) palette.b.y = val;
            else if(!strcmp(info.text_keys[i], "pal_b_z")) palette.b.z = val;
            else if(!strcmp(info.text_keys[i], "pal_c_x")) palette.c.x = val;
            else if(!strcmp(info.text_keys[i], "pal_c_y")) palette.c.y = val;
            else if(!strcmp(info.text_keys[i], "pal_c_z")) palette.c.z = val;
            else if(!strcmp(info.text_keys[i], "pal_d_x")) palette.d.x = val;
            else if(!strcmp(info.text_keys[i], "pal_d_y")) palette.d.y = val;
            else if(!strcmp(info.text_keys[i], "pal_d_z")) palette.d.z = val;
        }



    }

};
#endif //ATTRACTORRENDERER_ATTRACTOR_H
