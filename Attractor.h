//
// Created by ethan on 7/25/23.
//

#ifndef ATTRACTORRENDERER_ATTRACTOR_H
#define ATTRACTORRENDERER_ATTRACTOR_H

#include "Buffer.h"
#include <cmath>

template <typename T>
struct Point {
    T x, y;
};

Point<float> clifford(Point<float> p, float a, float b, float c, float d) {
    return {
        std::sin(a * p.y) + c * std::cos(a * p.x),
        std::sin(b * p.x) + d * std::cos(b * p.y)
    };
}

struct Attractor {
    Buffer<unsigned> map { 1000, 1000 };
    Buffer<unsigned char> image { 1000 * 3, 1000 };

    int iterations { 1000000 };

    int max_exposure { 0 };

    float gamma {1.5};
    float exposure {0.8};

    float a = -1.4;
    float b = 1.6;
    float c = 1.0;
    float d = 0.7;

    void iterate() {
        Point<float> p { 0, 0 };

        Point<float> tr {2,2};
        Point<float> bl {-2, -2};

        Point<float> scale {
                (map.width() - 1) / (tr.x - bl.x),
                (map.height() - 1) / (tr.y - bl.y),

        };
        max_exposure = 0;

        for (int i=0;i<iterations;i++) {
//            if (log) { if (i % reset == 0) std::cout << "." << std::flush; }

            p = clifford(p, a, b, c, d);

            Point<int> plot = {
                    static_cast<int>((floor((-p.x - bl.x) * scale.x))),
                    static_cast<int>((floor((-p.y - bl.y) * scale.y)))
            };

            plot = {
                    std::max(0, std::min(plot.x, map.width()-1)),
                    std::max(0, std::min(plot.y, map.height()-1))
            };


            unsigned val = map.get(plot.x, plot.y) + 1;

            if (val > max_exposure) max_exposure = c;

            map.set(plot.x, plot.y, val);
        }
    }

    void develop() {
        for(int x=0;x<map.width();x++) {
            for(int y=0;y<map.height();y++) {
                auto val = static_cast<float>(map.get(x, y)) / static_cast<float>(max_exposure);

//                val = pow(val, 1.0 / gamma);
                val = std::max(0.0f, std::min(1.0f, val));

                image.set(3 * x + 0, y, val * 255.0);
                image.set(3 * x + 1, y, val * 255.0);
                image.set(3 * x + 2, y, val * 255.0);
            }
        }
    }

};
#endif //ATTRACTORRENDERER_ATTRACTOR_H
