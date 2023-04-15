#pragma once

#include "Vec3.h"

class Ray
{
public:
	explicit Ray() {}
	explicit Ray(const Point3& Origin, const Vec3& Direction, const float Time = 0.0f) 
		: m_Origin(Origin), m_Direction(Direction), m_Time(Time)
	{}

	Point3 Origin() const { return m_Origin; }
	Vec3 Direction() const { return m_Direction; }
	float Time() const { return m_Time; }

	Point3 At(const float T) const { return Origin() + (T * Direction()); }
private:
	Point3 m_Origin;
	Vec3 m_Direction;
	float m_Time;
};