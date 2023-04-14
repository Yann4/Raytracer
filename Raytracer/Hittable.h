#pragma once

#include "Ray.h"
#include "Vec3.h"
#include <memory>

class Material;

struct HitRecord
{
	bool HasHit;
	float T;
	Point3 Position;
	bool FrontFace;
	Vec3 Normal;
	std::shared_ptr<Material> HitMaterial;

	HitRecord() : HasHit(false), T(-1.0f), Position(), Normal(), HitMaterial(nullptr) {}
	HitRecord(const float T, const Point3& Pos, const Vec3& Normal, const Vec3& RayDirection, std::shared_ptr<Material> Material) : HasHit(true), T(T), Position(Pos),
		FrontFace(Dot(RayDirection, Normal) < 0.0f), Normal(FrontFace ? Normal : -Normal), HitMaterial(Material) {}
};

class IHittable
{
public:
	virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& outHit) = 0;
};