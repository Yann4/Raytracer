#pragma once

#include "Vec3.h"

void WriteColour(std::ostream& out, const Colour& colour)
{
	out << static_cast<int>(255.99 * colour.x()) << " "
		<< static_cast<int>(255.99 * colour.y()) << " "
		<< static_cast<int>(255.99 * colour.z()) << "\n";
}