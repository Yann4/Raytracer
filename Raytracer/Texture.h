#pragma once

#include "Common.h"
#include "Perlin.h"
#include "StbImg.h"

class Texture
{
public:
	virtual Colour Value(float u, float v, const Point3& p) const = 0;
};

class SolidColour : public Texture
{
public:
	SolidColour() {}
	SolidColour(const Colour Col) : m_Colour(Col) {}
	SolidColour(const float R, const float G, const float B) : m_Colour(R, G, B) {}

	virtual Colour Value(float u, float v, const Point3& p) const override { return m_Colour; }
private:
	Colour m_Colour;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture() {}
	CheckerTexture(std::shared_ptr<Texture> Even, std::shared_ptr<Texture> Odd) : m_Even(Even), m_Odd(Odd) {}
	CheckerTexture(const Colour& Even, const Colour& Odd) : m_Even(std::make_shared<SolidColour>(Even)), m_Odd(std::make_shared<SolidColour>(Odd)) {}

	virtual Colour Value(float u, float v, const Point3& p) const override
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

class NoiseTexture : public Texture
{
public:
	NoiseTexture(){}
	NoiseTexture(const float Scale) : m_Scale(Scale) {}

	virtual Colour Value(float u, float v, const Point3& p) const override
	{
		return Colour(1.0f) * 0.5f * (1.0f + std::sin((m_Scale*p.z()) + (10.0f * m_Noise.Turbulence(p * m_Scale))));
	}

private:
	Perlin m_Noise;
	float m_Scale;
};

class ImageTexture : public Texture
{
public:
	ImageTexture() {}
	ImageTexture(const char* FileName)
	{
		int componentsPerPixel = m_BytesPerPixel;

		m_Data = stbi_load(FileName, &m_Width, &m_Height, &componentsPerPixel, componentsPerPixel);
		if (m_Data == nullptr)
		{
			std::cerr << "Couldn't load image " << FileName << ".\n";
		}

		m_BytesPerScanline = m_BytesPerPixel * m_Width;
	}

	~ImageTexture()
	{
		if (m_Data) { delete m_Data; }
	}

	Colour Value(float u, float v, const Point3& p) const override
	{
		if (m_Data == nullptr)
		{
			return Colour{ 0.0f, 1.0f, 1.0f };
		}

		u = std::clamp(u, 0.0f, 1.0f);
		v = 1.0f - std::clamp(v, 0.0f, 1.0f);

		int i = static_cast<int>(u * m_Width);
		int j = static_cast<int>(v * m_Height);

		if (i >= m_Width) { i = m_Width - 1; }
		if (j >= m_Height) { j = m_Height - 1; }

		const float colourScale = 1.0f / 255.0f;
		const unsigned char* pixel = m_Data + (j * m_BytesPerScanline) + (i * m_BytesPerPixel);

		return Colour{ colourScale * pixel[0], colourScale * pixel[1], colourScale * pixel[2] };
	}

private:
	unsigned char* m_Data;
	int m_Width, m_Height;
	int m_BytesPerScanline;

	static constexpr int m_BytesPerPixel = 3;
};