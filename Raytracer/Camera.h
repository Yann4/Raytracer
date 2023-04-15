#pragma once
#include "Common.h"
#include "Ray.h"
#include "Vec3.h"

class Camera
{
public:
	Camera() {}
	Camera(const Point3& Position, const Point3& LookAt, const Vec3& Up,
			const float VFov, const float AspectRatio, const float Aperture, const float FocalLength,
			const float T0 = 0.0f, const float T1 = 0.0f) :
		m_FOV(VFov), m_AspectRatio(AspectRatio), m_FocalLength(FocalLength), m_Position(Position), m_Target(LookAt), m_Up(Up), m_Aperture(Aperture),
		m_T0(T0), m_T1(T1)
	{}

	Point3 Position()	const { return m_Position; }
	Vec3 Horizontal()	const { return FocalLength() * Width() * Normalised(Cross(Up(), Forward())); }
	Vec3 Vertical()		const
	{
		const Vec3 forward = Forward();
		return FocalLength() * Height() * Cross(forward, Normalised(Cross(Up(), forward)));
	}
	Vec3 Forward()		const { return Normalised(Position() - m_Target); }
	Vec3 Up()			const { return m_Up; }
	Vec3 LowerLeft()	const { return Position() - (Horizontal() / 2.0f) - (Vertical() / 2.0f) - (FocalLength() * Forward()); }
	float FocalLength() const { return m_FocalLength; }
	float Width()		const { return m_AspectRatio * Height(); }
	float Height()		const { return  2.0f * std::tanf(Common::DegreesToRadians(m_FOV) / 2.0f); }
	float LensRadius()	const { return m_Aperture / 2.0f; }
	Ray GetRay(const float s, const float t) const 
	{
		const Vec3 u = Normalised(Cross(Up(), Forward()));
		const Vec3 v = Cross(Forward(), u);
		const Vec3 random = LensRadius() * RandomInUnitDisk();
		const Vec3 offset = (u * random.x()) + (v * random.y());
		return Ray(Position() + offset, LowerLeft() + (s * Horizontal()) + (t * Vertical()) - Position() - offset, Common::Random(m_T0, m_T1));
	}
private:
	float m_FOV;
	float m_AspectRatio;
	float m_FocalLength;
	Point3 m_Position;
	Vec3 m_Target;
	Vec3 m_Up;
	float m_Aperture;
	float m_T0, m_T1;
};