#pragma once

#include <cmath>
#include <limits>
#include <random>

constexpr float Infinity = std::numeric_limits<float>::infinity();
constexpr float pi = 3.1415926535897932385f;

inline float DegreesToRadians(const float degrees) {
    return degrees * pi / 180.0f;
}

inline float Random01() {
	static std::uniform_real_distribution<float> distribution(0.0f, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}