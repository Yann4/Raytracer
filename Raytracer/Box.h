#pragma once

#include "AARect.h"
#include "Common.h"
#include "HittableList.h"

class Box : public IHittable
{
public:
	Box() {}
	Box(const Point3& P0, const Point3& P1, std::shared_ptr<Material> Mat) : m_Min(P0), m_Max(P1), m_Material(Mat)
	{
		m_Sides.Add(std::make_shared<XYRect>(P0.x(), P1.x(), P0.y(), P1.y(), P1.z(), Mat));
		m_Sides.Add(std::make_shared<XYRect>(P0.x(), P1.x(), P0.y(), P1.y(), P0.z(), Mat));

		m_Sides.Add(std::make_shared<XZRect>(P0.x(), P1.x(), P0.z(), P1.z(), P1.y(), Mat));
		m_Sides.Add(std::make_shared<XZRect>(P0.x(), P1.x(), P0.z(), P1.z(), P0.y(), Mat));

		m_Sides.Add(std::make_shared<YZRect>(P0.y(), P1.y(), P0.z(), P1.z(), P1.x(), Mat));
		m_Sides.Add(std::make_shared<YZRect>(P0.y(), P1.y(), P0.z(), P1.z(), P0.x(), Mat));
	}

	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const override
	{
		return m_Sides.Hit(R, TMin, TMax, OutHit);
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		OutBox = { m_Min, m_Max };
		return true;
	}
private:
	Point3 m_Min, m_Max;
	std::shared_ptr<Material> m_Material;
	HittableList m_Sides;
};