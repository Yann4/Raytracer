#pragma once

#include <cmath>
#include <limits>

constexpr float Infinity = std::numeric_limits<float>::infinity();
constexpr float pi = 3.1415926535897932385f;

inline float DegreesToRadians(const float degrees) {
    return degrees * pi / 180.0f;
}