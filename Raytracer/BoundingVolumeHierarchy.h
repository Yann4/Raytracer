#pragma once

#include "Common.h"
#include "Hittable.h"
#include "HittableList.h"

#include <algorithm>
#include <iostream>

inline bool BoxCompare(const std::shared_ptr<IHittable> A, const std::shared_ptr<IHittable> B, const int Axis)
{
	AABB BoxA, BoxB;

	if (!A->BoundingBox(0.0f, 0.0f, BoxA) || !B->BoundingBox(0.0f, 0.0f, BoxB))
	{
		std::cerr << "No bounding box found.\n";
	}

	return BoxA.Min()[Axis] < BoxB.Min()[Axis];
}

inline bool BoxXCompare(const std::shared_ptr<IHittable> A, const std::shared_ptr<IHittable> B)
{
	return BoxCompare(A, B, 0);
}

inline bool BoxYCompare(const std::shared_ptr<IHittable> A, const std::shared_ptr<IHittable> B)
{
	return BoxCompare(A, B, 1);
}

inline bool BoxZCompare(const std::shared_ptr<IHittable> A, const std::shared_ptr<IHittable> B)
{
	return BoxCompare(A, B, 2);
}

class BoundingVolumeHierarchy : public IHittable
{
public:
	BoundingVolumeHierarchy() {}
	BoundingVolumeHierarchy(const HittableList& List, const float T0 = 0.0f, const float T1 = 0.0f)
		: BoundingVolumeHierarchy(List.Objects(), 0, List.Objects().size(), T0, T1)
	{}
	BoundingVolumeHierarchy(const std::vector<std::shared_ptr<IHittable>>& Objects, const size_t Start, const size_t End, const float T0, const float T1)
	{
		std::vector<std::shared_ptr<IHittable>> objects = Objects; //Modifiable copy

		int axis = Common::RandomInt(0, 2);
		auto comparator = (axis == 0) ? BoxXCompare :
						  (axis == 1) ? BoxYCompare :
										BoxZCompare;

		size_t objectSpan = End - Start;

		if (objectSpan == 1)
		{
			m_Left = objects[Start];
			m_Right = objects[Start];
		}
		else if (objectSpan == 2)
		{
			if (comparator(objects[Start], objects[Start + 1]))
			{
				m_Left = objects[Start];
				m_Right = objects[Start + 1];
			}
			else
			{
				m_Left = objects[Start + 1];
				m_Right = objects[Start];
			}
		}
		else
		{
			std::sort(objects.begin() + Start, objects.begin() + End, comparator);
			size_t mid = Start + (objectSpan / 2);
			m_Left = std::make_shared<BoundingVolumeHierarchy>(objects, Start, mid, T0, T1);
			m_Right = std::make_shared<BoundingVolumeHierarchy>(objects, mid, End, T0, T1);
		}

		AABB leftBox, rightBox;
		if (!m_Left->BoundingBox(T0, T1, leftBox) || !m_Right->BoundingBox(T0, T1, rightBox))
		{
			std::cerr << "No bounding box found in BoundingVolumeHierarchy ctor.\n";
		}

		m_Box = { leftBox, rightBox };
	}

	virtual bool Hit(const Ray& R, float TMin, float TMax, HitRecord& OutHit) const override
	{
		if (!m_Box.Hit(R, TMin, TMax))
		{
			return false;
		}

		const bool hitLeft = m_Left->Hit(R, TMin, TMax, OutHit);
		const bool hitRight = m_Right->Hit(R, TMin, hitLeft ? OutHit.T : TMax, OutHit);

		return hitLeft || hitRight;
	}

	virtual bool BoundingBox(const float T0, const float T1, AABB& OutBox) const override
	{
		OutBox = m_Box;
		return true;
	}

private:
	std::shared_ptr<IHittable> m_Left;
	std::shared_ptr<IHittable> m_Right;
	AABB m_Box;
};