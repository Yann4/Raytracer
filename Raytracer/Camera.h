#pragma once
#include "Vec3.h"

class Camera
{
public:
	Camera() {}
	Camera(const Point3& Position, const float ViewportHeight, const float AspectRatio, const float FocalLength) :
		m_Height(ViewportHeight), m_AspectRatio(AspectRatio), m_FocalLength(FocalLength),
		m_Position(Position), m_Right(Width(), 0.0f, 0.0f), m_Up(0.0f, Height(), 0.0f)
	{}

	Point3 Position()	const { return m_Position; }
	Vec3 Right()		const { return m_Right; }
	Vec3 Up()			const { return m_Up; }
	Vec3 LowerLeft()	const { return Position() - (Right() / 2.0f) - (Up() / 2.0f) - Vec3(0.0f, 0.0f, FocalLength()); }
	float FocalLength() const { return m_FocalLength; }
	float Width()		const { return m_AspectRatio * m_Height; }
	float Height()		const { return m_Height; }

private:
	float m_Height;
	float m_AspectRatio;
	float m_FocalLength;
	Point3 m_Position;
	Vec3 m_Right;
	Vec3 m_Up;
};