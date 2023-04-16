#pragma once

#include "AABB.h"
#include "Ray.h"
#include "Vec3.h"
#include <memory>

class Material;

struct HitRecord
{
	bool HasHit;
	float T;
	float U, V;
	Point3 Position;
	bool FrontFace;
	Vec3 Normal;
	std::shared_ptr<Material> HitMaterial;

	HitRecord() : HasHit(false), T(-1.0f), Position(), Normal(), HitMaterial(nullptr) {}
	HitRecord(const float T, const Point3& Pos, const Vec3& Normal, const Vec3& RayDirection, std::shared_ptr<Material> Material,
		const float u, const float v) 
		: HasHit(true), T(T), Position(Pos), FrontFace(Dot(RayDirection, Normal) < 0.0f), Normal(FrontFace ? Normal : -Normal), 
		HitMaterial(Material), U(u), V(v)
	{}

	void SetFaceNormal(const Ray& R, const Point3& FaceNormal)
	{
		FrontFace = Dot(R.Direction(), Normal) < 0.0f;
		Normal = FrontFace ? FaceNormal : -FaceNormal;
	}
};

class IHittable
{
public:
	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const = 0;
	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const = 0;
};

class Translate : public IHittable
{
public:
	Translate(std::shared_ptr<IHittable> Subject, const Vec3& Displacement) : m_Object(Subject), m_Displacement(Displacement)
	{}

	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const override
	{
		Ray moved(R.Origin() - m_Displacement, R.Direction(), R.Time());
		if (!m_Object->Hit(moved, TMin, TMax, OutHit))
		{
			return false;
		}

		OutHit.Position += m_Displacement;
		OutHit.SetFaceNormal(R, OutHit.Normal);
		return true;
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		if (!m_Object->BoundingBox(T0, T1, OutBox))
		{
			return false;
		}

		OutBox = { OutBox.Min() + m_Displacement, OutBox.Max() + m_Displacement };
		return true;
	}
private:
	std::shared_ptr<IHittable> m_Object;
	Point3 m_Displacement;
};

class RotateY : public IHittable
{
public:
	RotateY(std::shared_ptr<IHittable> Subject, const float Theta) : m_Object(Subject)
	{
		const float radians = Common::DegreesToRadians(Theta);
		m_SinTheta = std::sin(radians);
		m_CosTheta = std::cos(radians);
		m_HasBox = m_Object->BoundingBox(0.0f, 1.0f, m_Box);

		Point3 min(Common::Infinity);
		Point3 max(-Common::Infinity);

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					const float x = (i * m_Box.Max().x()) + (1 - i) * m_Box.Min().x();
					const float y = (j * m_Box.Max().y()) + (1 - j) * m_Box.Min().y();
					const float z = (k * m_Box.Max().z()) + (1 - k) * m_Box.Min().z();

					const float newX = (m_CosTheta * x) + (m_SinTheta * z);
					const float newZ = -(m_SinTheta * x) + (m_CosTheta * z);

					Vec3 tester(newX, y, newZ);
					for (int c = 0; c < 3; c++)
					{
						min[c] = std::fminf(min[c], tester[c]);
						max[c] = std::fmaxf(min[c], tester[c]);
					}
				}
			}
		}

		m_Box = { min, max };
	}

	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const override
	{
		Point3 origin = R.Origin();
		Vec3 direction = R.Direction();

		origin[0] = (m_CosTheta * R.Origin()[0]) - (m_SinTheta * R.Origin()[2]);
		origin[2] = (m_SinTheta * R.Origin()[0]) + (m_CosTheta * R.Origin()[2]);
		
		direction[0] = (m_CosTheta * R.Direction()[0]) - (m_SinTheta * R.Direction()[2]);
		direction[2] = (m_SinTheta * R.Direction()[0]) + (m_CosTheta * R.Direction()[2]);

		Ray rotated(origin, direction, R.Time());
		if (!m_Object->Hit(rotated, TMin, TMax, OutHit))
		{
			return false;
		}

		Point3 hitPos = OutHit.Position;
		Vec3 hitNormal = OutHit.Normal;

		hitPos[0] = (m_CosTheta * OutHit.Position[0]) + (m_SinTheta * OutHit.Position[2]);
		hitPos[2] = -(m_SinTheta * OutHit.Position[0]) + (m_CosTheta * OutHit.Position[2]);

		hitNormal[0] = (m_CosTheta * OutHit.Normal[0]) + (m_SinTheta * OutHit.Normal[2]);
		hitNormal[2] = -(m_SinTheta * OutHit.Normal[0]) + (m_CosTheta * OutHit.Normal[2]);

		OutHit.Position = hitPos;
		OutHit.SetFaceNormal(rotated, hitNormal);

		return true;
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		OutBox = m_Box;
		return m_HasBox;
	}

private:
	std::shared_ptr<IHittable> m_Object;
	float m_SinTheta, m_CosTheta;
	bool m_HasBox;
	AABB m_Box;
};