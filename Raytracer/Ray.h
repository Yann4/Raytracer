#pragma once

#include "Vec3.h"

struct Sphere
{
	Point3 Centre;
	float Radius;
};

struct Hit
{
	bool HasHit;
	float T;
	Point3 Position;
	bool FrontFace;
	Vec3 Normal;

	Hit() : HasHit(false), T(-1.0f), Position(), Normal() {}
	Hit(const float T, const Point3& Pos, const Vec3& Normal, const Vec3& RayDirection) : HasHit(true), T(T), Position(Pos),
		FrontFace(Dot(R.Direction(), OutwardNormal) < 0.0f), Normal(FrontFace ? OutwardNormal : -OutwardNormal) {}
};

class Ray
{
public:
	explicit Ray() {}
	explicit Ray(const Point3& Origin, const Vec3& Direction) : m_Origin(Origin), m_Direction(Direction) {}

	Point3 Origin() const { return m_Origin; }
	Vec3 Direction() const { return m_Direction; }

	Point3 At(const float T) const { return Origin() + (T * Direction()); }

	bool Intersects(const Sphere& Sphere, const float tMin, const float tMax, Hit& OutHit) const
	{
		Point3 Oc = Origin() - Sphere.Centre;

		const float a = Direction().LengthSq();
		const float halfB = Dot(Oc, Direction());
		const float c = Oc.LengthSq() - (Sphere.Radius * Sphere.Radius);
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

			const Point3 pos = At(root);
			const Vec3 OutNormal = (pos - Sphere.Centre) / Sphere.Radius;
			OutHit = { root, pos, OutNormal, Direction() };
			return true;
		}
	}
private:
	Point3 m_Origin;
	Vec3 m_Direction;
};