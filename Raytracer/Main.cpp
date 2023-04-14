#include "Camera.h"
#include "Common.h"
#include "Colour.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Ray.h"
#include "Vec3.h"

#include <iostream>

Colour RayColour(const Ray& R, const HittableList& world)
{
	HitRecord hit;
	if (world.Hit(R, 0.0f, Infinity, hit))
	{
		return 0.5f * (hit.Normal + Vec3(1.0f));
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

	HittableList world;
	world.Add(std::make_shared<Sphere>(Point3(0.0f, 0.0f, -1.0f), 0.5f));
	world.Add(std::make_shared<Sphere>(Point3(0.0f, -100.5f, -1.0f), 100.0f));

	Camera camera(Point3(0.0f), 2.0f, AspectRatio, 1.0f);

	std::cout << "P3\n" << imgWidth << ' ' << imgHeight << "\n255\n";

	for (int j = imgHeight - 1; j >= 0; --j)
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

		for (int i = 0; i < imgWidth; i++)
		{
			Colour pixelColour{ 0.0f, 0.0f, 0.0f };
			for (int sample = 0; sample < SamplesPerPixel; sample++)
			{
				const float u = (static_cast<float>(i) + Random01()) / (imgWidth - 1);
				const float v = (static_cast<float>(j) + Random01()) / (imgHeight - 1);
				Ray ray = camera.GetRay(u, v);
				pixelColour += RayColour(ray, world);
			}

			WriteColour(std::cout, pixelColour, SamplesPerPixel);
		}
	}

	std::cerr << "\nDone.\n";
}