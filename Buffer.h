//
// Created by ethan on 7/26/23.
//

#ifndef ATTRACTORRENDERER_BUFFER_H
#define ATTRACTORRENDERER_BUFFER_H

#include <vector>

template<typename T>
struct Buffer {
    std::vector<T> array;

    Buffer(int width, int height) : m_width{width}, m_height{height} {
        array.resize(width * height);
    }
    T get(int x, int y) {
        return array.at(x + y * m_width);
    }
    inline void set(int x, int y, T val) {
        array[x + y * m_width] = val;
    }
    int width() {
        return m_width;
    }
    int height() {
        return m_height;
    }
    void fill(T v) {
        std::fill(array.begin(), array.end(), v);
    }
    private:
        int m_width, m_height;
};
#endif //ATTRACTORRENDERER_BUFFER_H
