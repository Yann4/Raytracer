#pragma once

#include "Common.h"
#include "Hittable.h"

class MovingSphere : public IHittable
{
public:
	MovingSphere(){}
	MovingSphere(const Point3& Centre0, const Point3& Centre1, const float T0, const float T1, const float Radius, std::shared_ptr<Material> Mat)
		: m_Centre0(Centre0), m_Centre1(Centre1), m_T0(T0), m_T1(T1), m_Radius(Radius), m_Material(Mat)
	{}

	Point3 Position(const float Time) const { return m_Centre0 + ((Time - m_T0) / (m_T1 - m_T0)) * (m_Centre1 - m_Centre0); }

	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const override
	{
		Point3 Oc = R.Origin() - Position(R.Time());

		const float a = R.Direction().LengthSq();
		const float halfB = Dot(Oc, R.Direction());
		const float c = Oc.LengthSq() - (m_Radius * m_Radius);
		const float discriminant = (halfB * halfB) - (a * c);

		if (discriminant < 0)
		{
			return false;
		}
		else
		{
			const float sqrtD = std::sqrt(discriminant);

			float root = (-halfB - sqrtD) / a;
			if (root < TMin || TMax < root)
			{
				root = (-halfB + sqrtD) / a;
				if (root < TMin || TMax < root)
				{
					return false;
				}
			}

			const Point3 rayPos = R.At(root);
			const Vec3 outNormal = (rayPos - Position(R.Time())) / m_Radius;
			OutHit = { root, rayPos, outNormal, R.Direction(), m_Material };
			return true;
		}
	}

	bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		AABB box0(Position(T0) - Vec3(m_Radius), Position(T0) + Vec3(m_Radius));
		AABB box1(Position(T1) - Vec3(m_Radius), Position(T1) + Vec3(m_Radius));

		OutBox = AABB(box0, box1);
		return true;
	}

private:
	Point3 m_Centre0, m_Centre1;
	float m_T0, m_T1;
	float m_Radius;
	std::shared_ptr<Material> m_Material;
};