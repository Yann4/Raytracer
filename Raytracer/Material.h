#pragma once

#include "Common.h"
#include "Colour.h"
#include "Hittable.h"
#include "Ray.h"

struct HitRecord;

class Material
{
public:
	virtual bool Scatter(const Ray& R, const HitRecord& Hit, Colour& Attenuation, Ray& Scattered) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const Colour& Albedo) : m_Albedo(Albedo) {}

	virtual bool Scatter(const Ray& R, const HitRecord& Hit, Colour& Attenuation, Ray& Scattered) const override
	{
		Vec3 scatterDirection = Hit.Normal + RandomUnitVector();

		//Catch degenerate scatter direction
		if (scatterDirection.NearZero())
		{
			scatterDirection = Hit.Normal;
		}

		Scattered = Ray(Hit.Position, scatterDirection);
		Attenuation = m_Albedo;
		return true;
	}

private:
	Colour m_Albedo;
};

class Metal : public Material
{
public:
	Metal(const Colour& Albedo, const float Fuzziness) : m_Albedo(Albedo), m_Fuzziness(Fuzziness < 1 ? Fuzziness : 1.0f) {}

	virtual bool Scatter(const Ray& R, const HitRecord& Hit, Colour& Attenuation, Ray& Scattered) const override
	{
		Vec3 reflected = Reflect(Normalised(R.Direction()), Hit.Normal);
		Scattered = Ray(Hit.Position, reflected + (m_Fuzziness*RandomInUnitSphere()));
		Attenuation = m_Albedo;

		return (Dot(Scattered.Direction(), Hit.Normal) > 0.0f);
	}
private:
	Colour m_Albedo;
	float m_Fuzziness;
};