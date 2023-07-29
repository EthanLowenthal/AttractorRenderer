//
// Created by ethan on 7/29/23.
//

#ifndef ATTRACTORRENDERER_PALETTE_H
#define ATTRACTORRENDERER_PALETTE_H

#include <cmath>
#include <vector>
#include "vector.h"

struct Palette {
    vec3<float> a, b, c, d;
};

vec3<double> get_color(float t, Palette& p) {
    return {
        p.a.x + p.b.x * std::cos( 6.28318 * (p.c.x * t + p.d.x)),
        p.a.y + p.b.y * std::cos( 6.28318 * (p.c.y * t + p.d.y)),
        p.a.z + p.b.z * std::cos( 6.28318 * (p.c.z * t + p.d.z))
    };
}

std::vector<unsigned char> gen_palette_img(Palette& p, int width) {
    std::vector<unsigned char> image;
    image.resize(width * 3);

    for (float i=0;i<width;i++) {
        auto color = get_color(i / width, p);
        image[i * 3 + 0] = color.x * 255;
        image[i * 3 + 1] = color.y * 255;
        image[i * 3 + 2] = color.z * 255;
    }

    return image;
}

#endif //ATTRACTORRENDERER_PALETTE_H
