#include "Camera.h"
#include "Common.h"
#include "Colour.h"
#include "HittableList.h"
#include "Material.h"
#include "Sphere.h"
#include "Ray.h"
#include "Vec3.h"

#include <iostream>

Colour RayColour(const Ray& R, const HittableList& world, int Depth)
{
	if (Depth <= 0)
	{
		return Colour{ 0.0f };
	}

	HitRecord hit;
	if (world.Hit(R, 0.001f, Common::Infinity, hit))
	{
		Ray scattered;
		Colour attenuation;
		if (hit.HitMaterial->Scatter(R, hit, attenuation, scattered))
		{
			return attenuation * RayColour(scattered, world, Depth - 1);
		}

		return Colour{ 0.0f };
	}

	Vec3 unitDir = Normalised(R.Direction());
	float t = 0.5f * (unitDir.y() + 1.0f);
	return (1.0f - t) * Colour(1.0f) + t * Colour(0.5f, 0.7f, 1.0f);
}

HittableList CoverScene()
{
	HittableList world;

	std::shared_ptr<Material> groundMat = std::make_shared<Lambertian>(Colour(0.5f, 0.5f, 0.5f));
	world.Add(std::make_shared<Sphere>(Point3(0.0f, -1000.0f, 0.0f), 1000.0f, groundMat));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float chooseMat = Common::Random();
			Point3 centre(a + 0.9f * Common::Random(), 0.2f, b + 0.9f * Common::Random());

			if ((centre - Point3(4.0f, 0.2f, 0.0f)).Length() > 0.9f)
			{
				std::shared_ptr<Material> mat;

				if (chooseMat < 0.8f)
				{
					//Diffuse
					Colour albedo = Colour::Random() * Colour::Random();
					mat = std::make_shared<Lambertian>(albedo);
					world.Add(std::make_shared<Sphere>(centre, 0.2f, mat));
				}
				else if (chooseMat < 0.95f)
				{
					//Metal
					Colour albedo = Colour::Random(0.5f, 1.0f);
					float fuzz = Common::Random(0.0f, 0.5f);
					mat = std::make_shared<Metal>(albedo, fuzz);
					world.Add(std::make_shared<Sphere>(centre, 0.2f, mat));
				}
				else
				{
					//Glass
					mat = std::make_shared<Dielectric>(1.5f);
					world.Add(std::make_shared<Sphere>(centre, 0.2f, mat));
				}
			}
		}
	}

	std::shared_ptr<Material> material1 = std::make_shared<Dielectric>(1.5f);
	world.Add(std::make_shared<Sphere>(Point3(0.0f, 1.0f, 0.0f), 1.0f, material1));

	std::shared_ptr<Material> material2 = std::make_shared<Lambertian>(Colour(0.4f, 0.2f, 0.1f));
	world.Add(std::make_shared<Sphere>(Point3(-4.0f, 1.0f, 0.0f), 1.0f, material2));

	std::shared_ptr<Material> material3 = std::make_shared<Metal>(Colour(0.7f, 0.6f, 0.5f), 0.0f);
	world.Add(std::make_shared<Sphere>(Point3(4.0f, 1.0f, 0.0f), 1.0f, material3));

	return world;
}

int main(int argc, char** argv)
{
	constexpr float AspectRatio = 3.0f / 2.0f;
	constexpr unsigned int imgWidth = 1200;
	constexpr unsigned int imgHeight = static_cast<int>(imgWidth / AspectRatio);
	constexpr int SamplesPerPixel = 500;
	constexpr int MaxDepth = 50;

	HittableList world = CoverScene();

	const Point3 lookFrom = Point3(13.0f, 2.0f, 3.0f);
	const Point3 lookAt = Point3(0.0f, 0.0f, 0.0f);
	const Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
	const float focalDistance = 10.0f;
	const float aperture = 0.1f;

	Camera camera(lookFrom, lookAt, up, 20.0f, AspectRatio, aperture, focalDistance);

	std::cout << "P3\n" << imgWidth << ' ' << imgHeight << "\n255\n";

	for (int j = imgHeight - 1; j >= 0; --j)
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

		for (int i = 0; i < imgWidth; i++)
		{
			Colour pixelColour{ 0.0f, 0.0f, 0.0f };
			for (int sample = 0; sample < SamplesPerPixel; sample++)
			{
				const float u = (static_cast<float>(i) + Common::Random()) / (imgWidth - 1);
				const float v = (static_cast<float>(j) + Common::Random()) / (imgHeight - 1);
				Ray ray = camera.GetRay(u, v);
				pixelColour += RayColour(ray, world, MaxDepth);
			}

			WriteColour(std::cout, pixelColour, SamplesPerPixel);
		}
	}

	std::cerr << "\nDone.\n";
}