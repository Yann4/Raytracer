#pragma once

#include "Common.h"
#include "Colour.h"
#include "Hittable.h"
#include "Ray.h"
#include "Texture.h"

struct HitRecord;

class Material
{
public:
	virtual bool Scatter(const Ray& R, const HitRecord& Hit, Colour& Attenuation, Ray& Scattered) const = 0;
	virtual Colour Emit(const float U, const float V, const Point3& P) const { return Colour{ 0.0f }; }
};

class Lambertian : public Material
{
public:
	Lambertian(const Colour& Albedo) : m_Albedo(std::make_shared<SolidColour>(Albedo)) {}
	Lambertian(const std::shared_ptr<Texture> Albedo) : m_Albedo(Albedo) {}

	virtual bool Scatter(const Ray& R, const HitRecord& Hit, Colour& Attenuation, Ray& Scattered) const override
	{
		Vec3 scatterDirection = Hit.Normal + RandomUnitVector();

		//Catch degenerate scatter direction
		if (scatterDirection.NearZero())
		{
			scatterDirection = Hit.Normal;
		}

		Scattered = Ray(Hit.Position, scatterDirection, R.Time());
		Attenuation = m_Albedo->Value(Hit.U, Hit.V, Hit.Position);
		return true;
	}

private:
	std::shared_ptr<Texture> m_Albedo;
};

class Metal : public Material
{
public:
	Metal(const Colour& Albedo, const float Fuzziness) : m_Albedo(Albedo), m_Fuzziness(Fuzziness < 1 ? Fuzziness : 1.0f) {}

	virtual bool Scatter(const Ray& R, const HitRecord& Hit, Colour& Attenuation, Ray& Scattered) const override
	{
		Vec3 reflected = Reflect(Normalised(R.Direction()), Hit.Normal);
		Scattered = Ray(Hit.Position, reflected + (m_Fuzziness*RandomInUnitSphere()), R.Time());
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

		Scattered = Ray(Hit.Position, direction, R.Time());
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

class DiffuseLight : public Material
{
public:
	DiffuseLight(std::shared_ptr<Texture> Emit) : m_Emit(Emit) {}
	DiffuseLight(const Colour& Emit) : DiffuseLight(std::make_shared<SolidColour>(Emit)) {}

	virtual bool Scatter(const Ray& R, const HitRecord& Hit, Colour& Attenuation, Ray& Scattered) const override { return false; }
	virtual Colour Emit(const float u, const float v, const Point3& P) const override
	{
		return m_Emit->Value(u, v, P);
	}

private:
	std::shared_ptr<Texture> m_Emit;
};

class Isotropic : public Material
{
public:
	Isotropic(std::shared_ptr<Texture> Tex) : m_Albedo(Tex) {}
	Isotropic(const Colour& Albedo) : Isotropic(std::make_shared<SolidColour>(Albedo)) {}

	virtual bool Scatter(const Ray& R, const HitRecord& Hit, Colour& Attenuation, Ray& Scattered) const override
	{
		Scattered = Ray(Hit.Position, RandomInUnitSphere(), R.Time());
		Attenuation = m_Albedo->Value(Hit.U, Hit.V, Hit.Position);

		return true;
	}

private:
	std::shared_ptr<Texture> m_Albedo;
};