#pragma once

#include "Common.h"
#include "Hittable.h"
#include "Material.h"

class ConstantMedium : public IHittable
{
public:
	ConstantMedium(std::shared_ptr<IHittable> Boundary, const float Density, std::shared_ptr<Texture> Tex)
		: m_Boundary(Boundary), m_NegInvDensity(-1.0f / Density), m_PhaseFunction(std::make_shared<Isotropic>(Tex))
	{}
	ConstantMedium(std::shared_ptr<IHittable> Boundary, const float Density, const Colour& Col)
		: m_Boundary(Boundary), m_NegInvDensity(-1.0f / Density), m_PhaseFunction(std::make_shared<Isotropic>(Col))
	{}

	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const override
	{
		const bool enableDebug = false;
		const bool debug = enableDebug && Common::Random() < 0.00001f;

		HitRecord hit1, hit2;
		if (!m_Boundary->Hit(R, -Common::Infinity, Common::Infinity, hit1))
		{
			return false;
		}

		if (!m_Boundary->Hit(R, hit1.T + 0.0001f, Common::Infinity, hit2))
		{
			return false;
		}

		if (debug)
		{
			std::cerr << "\nTMin=" << hit1.T << ", TMax=" << hit2.T << "\n";
		}

		if (hit1.T < TMin) { hit1.T = TMin; }
		if (hit2.T > TMax) { hit2.T = TMax; }

		if (hit1.T >= hit2.T) { return false; }
		if (hit1.T < 0.0f) { hit1.T = 0.0f; }

		const float rayLength = R.Direction().Length();
		const float distWithinBoundary = (hit2.T - hit1.T) * rayLength;
		const float hitDistance = m_NegInvDensity * std::log(Common::Random());

		if (hitDistance > distWithinBoundary) { return false; }

		const float t = hit1.T + hitDistance / rayLength;
		OutHit.T = t;
		OutHit.Position = R.At(t);
		OutHit.Normal = Vec3(1.0f, 0.0f, 0.0f);
		OutHit.FrontFace = true;
		OutHit.HitMaterial = m_PhaseFunction;

		if (debug)
		{
			std::cerr << "hit distance = " << hitDistance << "\nT = " << t << "\npos = " << OutHit.Position << "\n";
		}

		return true;
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		return m_Boundary->BoundingBox(T0, T1, OutBox);
	}
private:
	std::shared_ptr<IHittable> m_Boundary;
	std::shared_ptr<Material> m_PhaseFunction;
	float m_NegInvDensity;
};