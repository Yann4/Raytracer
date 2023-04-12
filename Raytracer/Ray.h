#pragma once

#include "Vec3.h"

class Ray
{
public:
	explicit Ray() {}
	explicit Ray(const Point3& Origin, const Vec3& Direction) : m_Origin(Origin), m_Direction(Direction) {}

	Point3 Origin() const { return m_Origin; }
	Vec3 Direction() const { return m_Direction; }

	Point3 At(const float T) const { return Origin() + (T * Direction()); }
private:
	Point3 m_Origin;
	Vec3 m_Direction;
};