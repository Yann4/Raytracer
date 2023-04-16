#pragma once

#include "Common.h"
#include "Hittable.h"

class XYRect : public IHittable
{
public:
	XYRect() {}
	XYRect(const float x0, const float x1, const float y0, const float y1, const float k, std::shared_ptr<Material> Mat)
		: m_X0(x0), m_X1(x1), m_Y0(y0), m_Y1(y1), m_K(k), m_Material(Mat)
	{}

	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const override
	{
		const float t = (m_K - R.Origin().z()) / R.Direction().z();
		if (t < TMin || t > TMax)
		{
			return false;
		}

		const float x = R.Origin().x() + (t * R.Direction().x());
		const float y = R.Origin().y() + (t * R.Direction().y());
		if (x < m_X0 || x > m_X1 || y < m_Y0 || y > m_Y1)
		{
			return false;
		}

		const float u = (x - m_X0) / (m_X1 - m_X0);
		const float v = (y - m_Y0) / (m_Y1 - m_Y0);

		OutHit = { t, R.At(t), Vec3{0.0f, 0.0f, 1.0f}, R.Direction(), m_Material, u, v };
		return true;
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		OutBox = { Point3(m_X0, m_Y0, m_K - 0.0001f), Point3(m_X1, m_Y1, m_K + 0.0001f) };
		return true;
	}

private:
	float m_X0, m_X1;
	float m_Y0, m_Y1;
	float m_K;
	std::shared_ptr<Material> m_Material;
};

class YZRect : public IHittable
{
public:
	YZRect() {}
	YZRect(const float y0, const float y1, const float z0, const float z1, const float k, std::shared_ptr<Material> Mat)
		: m_Y0(y0), m_Y1(y1), m_Z0(y0), m_Z1(z1), m_K(k), m_Material(Mat)
	{}

	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const override
	{
		const float t = (m_K - R.Origin().x()) / R.Direction().x();
		if (t < TMin || t > TMax)
		{
			return false;
		}

		const float y = R.Origin().y() + (t * R.Direction().y());
		const float z = R.Origin().z() + (t * R.Direction().z());
		if (y < m_Y0 || y > m_Y1 || z < m_Z0 || z > m_Z1)
		{
			return false;
		}

		const float u = (y - m_Y0) / (m_Y1 - m_Y0);
		const float v = (z - m_Z0) / (m_Z1 - m_Z0);

		OutHit = { t, R.At(t), Vec3{1.0f, 0.0f, 0.0f}, R.Direction(), m_Material, u, v };
		return true;
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		OutBox = { Point3(m_K - 0.0001f, m_Y0, m_Z0), Point3(m_K + 0.0001f, m_Y1, m_Z1) };
		return true;
	}

private:
	float m_Y0, m_Y1;
	float m_Z0, m_Z1;
	float m_K;
	std::shared_ptr<Material> m_Material;
};

class XZRect : public IHittable
{
public:
	XZRect() {}
	XZRect(const float x0, const float x1, const float z0, const float z1, const float k, std::shared_ptr<Material> Mat)
		: m_X0(x0), m_X1(x1), m_Z0(z0), m_Z1(z1), m_K(k), m_Material(Mat)
	{}

	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const override
	{
		const float t = (m_K - R.Origin().y()) / R.Direction().y();
		if (t < TMin || t > TMax)
		{
			return false;
		}

		const float x = R.Origin().x() + (t * R.Direction().x());
		const float z = R.Origin().z() + (t * R.Direction().z());
		if (x < m_X0 || x > m_X1 || z < m_Z0 || z > m_Z1)
		{
			return false;
		}

		const float u = (x - m_X0) / (m_X1 - m_X0);
		const float v = (z - m_Z0) / (m_Z1 - m_Z0);

		OutHit = { t, R.At(t), Vec3{0.0f, 1.0f, 0.0f}, R.Direction(), m_Material, u, v };
		return true;
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		OutBox = { Point3(m_X0, m_K - 0.0001f, m_Z0), Point3(m_X1, m_K + 0.0001f, m_Z1) };
		return true;
	}

private:
	float m_X0, m_X1;
	float m_Z0, m_Z1;
	float m_K;
	std::shared_ptr<Material> m_Material;
};