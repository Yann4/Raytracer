#pragma once

#include "Common.h"

class Texture
{
public:
	virtual Colour Value(const float u, const float v, const Point3& p) const = 0;
};

class SolidColour : public Texture
{
public:
	SolidColour() {}
	SolidColour(const Colour Col) : m_Colour(Col) {}
	SolidColour(const float R, const float G, const float B) : m_Colour(R, G, B) {}

	virtual Colour Value(const float u, const float v, const Point3& p) const override { return m_Colour; }
private:
	Colour m_Colour;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture() {}
	CheckerTexture(std::shared_ptr<Texture> Even, std::shared_ptr<Texture> Odd) : m_Even(Even), m_Odd(Odd) {}
	CheckerTexture(const Colour& Even, const Colour& Odd) : m_Even(std::make_shared<SolidColour>(Even)), m_Odd(std::make_shared<SolidColour>(Odd)) {}

	virtual Colour Value(const float u, const float v, const Point3& p) const override
	{
		const float sines = std::sinf(10.0f * p.x()) * std::sinf(10.0f * p.y()) * std::sinf(10.0f * p.z());
		if (sines < 0)
		{
			return m_Odd->Value(u, v, p);
		}
		else
		{
			return m_Even->Value(u, v, p);
		}
	}
private:
	std::shared_ptr<Texture> m_Even, m_Odd;
};