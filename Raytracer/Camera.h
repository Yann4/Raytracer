#pragma once
#include "Common.h"
#include "Ray.h"
#include "Vec3.h"

class Camera
{
public:
	Camera() {}
	Camera(const Point3& Position, const Point3& LookAt, const Vec3& Up, const float VFov, const float AspectRatio, const float FocalLength) :
		m_FOV(VFov), m_AspectRatio(AspectRatio), m_FocalLength(FocalLength), m_Position(Position), m_Target(LookAt), m_Up(Up)
	{}

	Point3 Position()	const { return m_Position; }
	Vec3 Horizontal()	const { return Width() * Normalised(Cross(Up(), Forward())); }
	Vec3 Vertical()		const
	{
		const Vec3 forward = Forward();
		return Height() * Cross(forward, Normalised(Cross(Up(), forward)));
	}
	Vec3 Forward()		const { return Normalised(Position() - m_Target); }
	Vec3 Up()			const { return m_Up; }
	Vec3 LowerLeft()	const { return Position() - (Horizontal() / 2.0f) - (Vertical() / 2.0f) - Forward(); }
	float FocalLength() const { return m_FocalLength; }
	float Width()		const { return m_AspectRatio * Height(); }
	float Height()		const { return  2.0f * std::tanf(Common::DegreesToRadians(m_FOV) / 2.0f); }

	Ray GetRay(const float s, const float t) const { return Ray(Position(), LowerLeft() + (s * Horizontal()) + (t * Vertical()) - Position()); }
private:
	float m_FOV;
	float m_AspectRatio;
	float m_FocalLength;
	Point3 m_Position;
	Vec3 m_Target;
	Vec3 m_Up;
};