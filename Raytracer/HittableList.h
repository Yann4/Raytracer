#pragma once

#include "Hittable.h"

#include <memory>
#include <vector>

class HittableList : public IHittable
{
public:
	HittableList() {}

	void Clear() { m_Objects.clear(); }
	void Add(std::shared_ptr<IHittable> Object) { m_Objects.emplace_back(Object); }

	virtual bool Hit(const Ray& Ray, const float TMin, const float TMax, HitRecord& OutHit) const override
	{
		HitRecord temp;
		bool anyHit = false;
		float closest = TMax;

		for (const std::shared_ptr<IHittable> object : m_Objects)
		{
			if (object->Hit(Ray, TMin, TMax, temp) && temp.T < closest)
			{
				anyHit = true;
				closest = temp.T;
				OutHit = temp;
			}
		}
		return anyHit;
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		if (m_Objects.empty())
		{
			return false;
		}

		AABB tempBox;
		bool firstBox = true;

		for (const std::shared_ptr<IHittable>& object : m_Objects)
		{
			if (!object->BoundingBox(T0, T1, tempBox))
			{
				return false;
			}

			OutBox = firstBox ? tempBox : AABB(OutBox, tempBox);
			firstBox = false;
		}

		return true;
	}

	std::vector<std::shared_ptr<IHittable>> Objects() const { return m_Objects; }
private:
	std::vector<std::shared_ptr<IHittable>> m_Objects;
};