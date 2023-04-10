#pragma once

#include <cmath>
#include <iostream>

class Vec3
{
public:
	explicit Vec3() : m_E{ 0,0,0 } {}
	explicit Vec3(float e0, float e1, float e2) : m_E{ e0, e1, e2 } {}
	explicit Vec3(float e0) : m_E{ e0, e0, e0 } {}

	float x() const { return m_E[0]; }
	float y() const { return m_E[1]; }
	float z() const { return m_E[2]; }

	Vec3 operator-() const { return Vec3(-m_E[0], -m_E[1], -m_E[2]); }
	float operator[](int i) const { return m_E[i]; }
	float& operator[](int i) { return m_E[i]; }

	Vec3& operator+=(const Vec3& rhs)
	{
		m_E[0] += rhs[0];
		m_E[1] += rhs[1];
		m_E[2] += rhs[2];
		return *this;
	}

	Vec3& operator*=(const float rhs)
	{
		m_E[0] *= rhs;
		m_E[1] *= rhs;
		m_E[2] *= rhs;
		return *this;
	}

	Vec3& operator/=(const float rhs)
	{
		return *this *= 1 / rhs;
	}

	float Length() const { return std::sqrt(LengthSq()); }
	float LengthSq() const { return m_E[0] * m_E[0] + m_E[1] * m_E[1] + m_E[2] * m_E[2]; }

private:
	float m_E[3];
};

using Point3 = Vec3;
using Colour = Vec3;

inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	return out << v[0] << " " << v[1] << " " << v[2];
}

inline Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
{
	return Vec3(lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z());
}

inline Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
{
	return Vec3(lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z());
}

inline Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
{
	return Vec3(lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z());
}

inline Vec3 operator*(const Vec3& lhs, const float rhs)
{
	return Vec3(lhs.x() * rhs, lhs.y() * rhs, lhs.z() * rhs);
}

inline Vec3 operator*(const float lhs, const Vec3& rhs)
{
	return Vec3(lhs * rhs.x(), lhs * rhs.y(), lhs * rhs.z());
}

inline Vec3 operator/(const Vec3& lhs, float rhs)
{
	return (1 / rhs) * lhs;
}

inline float Dot(const Vec3& u, const Vec3& v)
{
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

inline Vec3 Cross(const Vec3& u, const Vec3& v)
{
	return Vec3(u[1] * v[2] - u[2] * v[1],
				u[2] * v[0] - u[0] * v[2],
				u[0] * v[1] - u[1] * v[0]);
}

inline Vec3 Normalised(const Vec3& v)
{
	return v / v.Length();
}