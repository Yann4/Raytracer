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

class Dielectric : public Material
{
public:
	Dielectric(const float IndexOfRefraction) : m_IR(IndexOfRefraction) {}
	virtual bool Scatter(const Ray& R, const HitRecord& Hit, Colour& Attenuation, Ray& Scattered) const override
	{
		Attenuation = { 1.0f };
		float refractionRatio = Hit.FrontFace ? 1.0f / m_IR : m_IR;

		Vec3 unitDirection = Normalised(R.Direction());
		const float cosTheta = std::fminf(Dot(-unitDirection, Hit.Normal), 1.0f);
		const float sinTheta = std::sqrtf(1.0f - (cosTheta * cosTheta));

		const bool cannotRefract = refractionRatio * sinTheta > 1.0f;
		Vec3 direction;
		if (cannotRefract || Reflectance(cosTheta, refractionRatio) > Common::Random())
		{
			direction = Reflect(unitDirection, Hit.Normal);
		}
		else
		{
			direction = Refract(unitDirection, Hit.Normal, refractionRatio);
		}

		Scattered = Ray(Hit.Position, direction);
		return true;
	}

private:
	static float Reflectance(const float cosine, const float refIdx)
	{
		//Schlick's approximation
		float r0 = (1.0f - refIdx) / (1.0f + refIdx);
		r0 = r0 * r0;
		return r0 + ((1.0f - r0) * std::powf(1.0f - cosine, 5.0f));
	}
private:
	float m_IR;
};