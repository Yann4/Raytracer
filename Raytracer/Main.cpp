#include "Camera.h"
#include "Colour.h"
#include "Ray.h"
#include "Vec3.h"

#include <iostream>

Colour RayColour(const Ray& R)
{
	Hit hit;
	if (R.Intersects({ Point3(0.0f, 0.0f, -1.0f), 0.5f }, 0.0f, 1.0f, hit))
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

	Camera camera(Point3(0.0f), 2.0f, AspectRatio, 1.0f);

	std::cout << "P3\n" << imgWidth << ' ' << imgHeight << "\n255\n";

	for (int j = imgHeight - 1; j >= 0; --j)
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

		for (int i = 0; i < imgWidth; i++)
		{
			const float u = static_cast<float>(i) / (imgWidth - 1);
			const float v = static_cast<float>(j) / (imgHeight - 1);
			Ray ray(camera.Position(), camera.LowerLeft() + (u * camera.Right()) + (v * camera.Up()) - camera.Position());
			Colour pixelColour = RayColour(ray);
			WriteColour(std::cout, pixelColour);
		}
	}

	std::cerr << "\nDone.\n";
}