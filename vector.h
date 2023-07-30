//
// Created by ethan on 7/29/23.
//

#ifndef ATTRACTORRENDERER_VECTOR_H
#define ATTRACTORRENDERER_VECTOR_H

template <typename T>
struct vec2 {
    T x, y;
    T& operator[](int index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Invalid index in vec2");
        }
    }
};
template <typename T>
struct vec3 {
    T x, y, z;
    T& operator[](int index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Invalid index in vec3");
        }
    }
};

#endif //ATTRACTORRENDERER_VECTOR_H
