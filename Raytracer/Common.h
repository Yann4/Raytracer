#pragma once

#include <cmath>
#include <limits>
#include <random>

namespace Common
{
	constexpr float Infinity = std::numeric_limits<float>::infinity();
	constexpr float pi = 3.1415926535897932385f;

	inline float DegreesToRadians(const float degrees) {
		return degrees * pi / 180.0f;
	}

	inline float Random() {
		static std::uniform_real_distribution<float> distribution(0.0f, 1.0);
		static std::mt19937 generator;
		return distribution(generator);
	}

	inline float Random(const float Min, const float Max)
	{
		// Returns a random real in [min,max).
		return Min + ((Max - Min) * Common::Random());
	}

	inline int RandomInt(const int Min, const int Max)
	{
		return static_cast<int>(Random(static_cast<float>(Min), static_cast<float>(Max + 1)));
	}
}