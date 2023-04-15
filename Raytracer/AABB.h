#pragma once

#include "Common.h"
#include "Ray.h"
#include "Vec3.h"

class AABB
{
public:
	AABB() {}
	AABB(const Point3& Min, const Point3& Max) : m_Max(Max), m_Min(Min) {}
	AABB(const AABB& A, const AABB& B)
	{
		m_Min = { std::fminf(A.Min().x(), B.Min().x()), std::fminf(A.Min().y(), B.Min().y()), std::fminf(A.Min().z(), B.Min().z()) };
		m_Max = { std::fmaxf(A.Max().x(), B.Max().x()), std::fmaxf(A.Max().y(), B.Max().y()), std::fmaxf(A.Max().z(), B.Max().z()) };
	}

	Point3 Max() const { return m_Max; }
	Point3 Min() const { return m_Min; }

	bool Hit(const Ray& R, float TMin, float TMax) const
	{
		for (int axis = 0; axis < 3; axis++)
		{
			const float inverseDirection = 1.0f / R.Direction()[axis];
			float t0 = (Min()[axis] - R.Origin()[axis]) * inverseDirection;
			float t1 = (Max()[axis] - R.Origin()[axis]) * inverseDirection;
			if (inverseDirection < 0.0f)
			{
				std::swap(t0, t1);
			}

			TMin = t0 > TMin ? t0 : TMin;
			TMax = t1 < TMax ? t1 : TMax;

			if (TMax <= TMin)
			{
				return false;
			}
		}

		return true;
	}
private:
	Point3 m_Max, m_Min;
};