#pragma once

#include "Hittable.h"

#include <memory>
#include <vector>

class HittableList
{
public:
	HittableList() {}

	void Clear() { m_Objects.clear(); }
	void Add(std::shared_ptr<IHittable> Object) { m_Objects.emplace_back(Object); }

	bool Hit(const Ray& Ray, const float TMin, const float TMax, HitRecord& OutHit) const
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

private:
	std::vector<std::shared_ptr<IHittable>> m_Objects;
};