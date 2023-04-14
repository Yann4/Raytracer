#pragma once

#include "Hittable.h"
#include "Vec3.h"

class Sphere : public IHittable
{
public:
	Sphere() {}
	Sphere(const Point3& Centre, const float Radius) : m_Centre(Centre), m_Radius(Radius) {}

	//IHittable
	virtual bool Hit(const Ray& Ray, float tMin, float tMax, HitRecord& OutHit) override
	{
		Point3 Oc = Ray.Origin() - m_Centre;

		const float a = Ray.Direction().LengthSq();
		const float halfB = Dot(Oc, Ray.Direction());
		const float c = Oc.LengthSq() - (m_Radius * m_Radius);
		const float discriminant = (halfB * halfB) - (a * c);

		if (discriminant < 0)
		{
			OutHit = {};
			return false;
		}
		else
		{
			const float sqrtD = std::sqrt(discriminant);

			float root = (-halfB - sqrtD) / a;
			if (root < tMin || tMax < root)
			{
				root = (-halfB + sqrtD) / a;
				if (root < tMin || tMax < root)
				{
					OutHit = {};
					return false;
				}
			}

			const Point3 pos = Ray.At(root);
			const Vec3 OutNormal = (pos - m_Centre) / m_Radius;
			OutHit = { root, pos, OutNormal, Ray.Direction() };
			return true;
		}
	}

	Point3 Centre() const { return m_Centre; }
	float Radius() const { return m_Radius; }
private:
	Point3 m_Centre;
	float m_Radius;
};