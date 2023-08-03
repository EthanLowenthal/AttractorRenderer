//
// Created by ethan on 7/25/23.
//

#ifndef ATTRACTORRENDERER_ATTRACTOR_H
#define ATTRACTORRENDERER_ATTRACTOR_H

#include <cmath>

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
        unsigned error = lodepng::encode(filename + ".png", &image.array[0], size.x, size.y);

        //if there's an error, display it
        if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
    }

};
#endif //ATTRACTORRENDERER_ATTRACTOR_H
