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

int main(int argc, char** argv)
{
	constexpr float AspectRatio = 16.0f / 9.0f;
	constexpr unsigned int imgWidth = 400;
	constexpr unsigned int imgHeight = static_cast<int>(imgWidth / AspectRatio);
	constexpr int SamplesPerPixel = 100;
	constexpr int MaxDepth = 50;

	std::shared_ptr<Material> ground = std::make_shared<Lambertian>(Colour(0.8f, 0.8f, 0.0f));
	std::shared_ptr<Material> centre = std::make_shared<Lambertian>(Colour(0.1f, 0.2f, 0.5f));
	std::shared_ptr<Material> left = std::make_shared<Dielectric>(1.5f);
	std::shared_ptr<Material> right = std::make_shared<Metal>(Colour(0.8f, 0.6f, 0.2f), 0.0f);

	HittableList world;
	world.Add(std::make_shared<Sphere>(Point3(0.0f, -100.5f, -1.0f), 100.0f, ground));
	world.Add(std::make_shared<Sphere>(Point3(0.0f, 0.0f, -1.0f), 0.5f, centre));
	world.Add(std::make_shared<Sphere>(Point3(-1.0f, 0.0f, -1.0f), 0.5f, left));
	world.Add(std::make_shared<Sphere>(Point3(-1.0f, 0.0f, -1.0f), -0.4f, left));
	world.Add(std::make_shared<Sphere>(Point3(1.0f, 0.0f, -1.0f), 0.5f, right));

	Camera camera(Point3(-2.0f, 2.0f, 1.0f), Point3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f), 20.0f, AspectRatio, 1.0f);

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