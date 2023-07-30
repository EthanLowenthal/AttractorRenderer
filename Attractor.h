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
    Buffer<unsigned> map { 1000, 1000 };
    Buffer<unsigned char> image { 1000 * 4, 1000 };

    int iterations { 1000000 };

    int max_exposure { 0 };

    float gamma {1.5};
    float exposure {0.8};

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

        vec2<float> tr {2,2};
        vec2<float> bl {-2, -2};

        vec2<float> scale {
                (map.width() - 1) / (tr.x - bl.x),
                (map.height() - 1) / (tr.y - bl.y),
        };

        map.fill(0);

        max_exposure = 0;

        for (int i=0;i<iterations;i++) {
            progress++;
//            if (log) { if (i % reset == 0) std::cout << "." << std::flush; }

            p = clifford(p, a, b, c, d);

            vec2<int> plot = {
                    static_cast<int>((floor((-p.x - bl.x) * scale.x))),
                    static_cast<int>((floor((-p.y - bl.y) * scale.y)))
            };

            plot = {
                    std::max(0, std::min(plot.x, map.width()-1)),
                    std::max(0, std::min(plot.y, map.height()-1))
            };


            unsigned val = map.get(plot.x, plot.y) + 1;

            if (val > max_exposure) max_exposure = val;

            map.set(plot.x, plot.y, val);
        }
    }

    void develop() {
        for(int x=0;x<map.width();x++) {
            for(int y=0;y<map.height();y++) {
                auto val = static_cast<float>(map.get(x, y)) / static_cast<float>(max_exposure);

                val *= exposure;
                val = pow(val, 1.0 / gamma);
                val = std::max(0.0f, std::min(1.0f, val));

                auto color = get_color(val, palette);

                image.set(4 * x + 0, y, color.x * 255.0);
                image.set(4 * x + 1, y, color.y * 255.0);
                image.set(4 * x + 2, y, color.z * 255.0);
                image.set(4 * x + 3, y, 255.0);
            }
        }
    }

    void save(const std::string& filename) {
        unsigned error = lodepng::encode(filename + ".png", &image.array[0], map.width(), map.height());

        //if there's an error, display it
        if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
    }

};
#endif //ATTRACTORRENDERER_ATTRACTOR_H
