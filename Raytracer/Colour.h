#pragma once

#include "Vec3.h"
#include <cmath>

void WriteColour(std::ostream& out, const Colour& colour, const int SamplesPerPixel)
{
	const float scale = 1.0f / SamplesPerPixel;

	float r = colour.x() * scale;
	float g = colour.y() * scale;
	float b = colour.z() * scale;

	out << static_cast<int>(255.99 * std::clamp(r, 0.0f, 0.999f)) << " "
		<< static_cast<int>(255.99 * std::clamp(g, 0.0f, 0.999f)) << " "
		<< static_cast<int>(255.99 * std::clamp(b, 0.0f, 0.999f)) << "\n";
}