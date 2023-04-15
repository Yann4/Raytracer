#pragma once

#include "Hittable.h"
#include "Vec3.h"

#include <tuple>

class Sphere : public IHittable
{
public:
	Sphere() {}
	Sphere(const Point3& Centre, const float Radius, std::shared_ptr<Material> Material) : m_Centre(Centre), m_Radius(Radius), m_Material(Material) {}

	//IHittable
	virtual bool Hit(const Ray& Ray, float tMin, float tMax, HitRecord& OutHit) const override
	{
		Point3 Oc = Ray.Origin() - m_Centre;

		const float a = Ray.Direction().LengthSq();
		const float halfB = Dot(Oc, Ray.Direction());
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
			if (root < tMin || tMax < root)
			{
				root = (-halfB + sqrtD) / a;
				if (root < tMin || tMax < root)
				{
					return false;
				}
			}

			const Point3 pos = Ray.At(root);
			const Vec3 OutNormal = (pos - m_Centre) / m_Radius;
			const auto [u, v] = GetUV(OutNormal);
			OutHit = { root, pos, OutNormal, Ray.Direction(), Mat(), u, v };
			return true;
		}
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		OutBox = { Centre() - Vec3(m_Radius), Centre() + Vec3(m_Radius) };
		return true;
	}

	static std::tuple<float, float> GetUV(const Point3& P)
	{
		const float theta = std::acos(-P.y());
		const float phi = std::atan2(-P.z(), P.x()) + Common::pi;

		return { phi / (2 * Common::pi), theta / Common::pi };
	}

	Point3 Centre() const { return m_Centre; }
	float Radius() const { return m_Radius; }
	std::shared_ptr<Material> Mat() const { return m_Material; }
private:
	Point3 m_Centre;
	float m_Radius;
	std::shared_ptr<Material> m_Material;
};