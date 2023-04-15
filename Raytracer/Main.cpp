#include "BoundingVolumeHierarchy.h"
#include "Camera.h"
#include "Common.h"
#include "Colour.h"
#include "HittableList.h"
#include "Material.h"
#include "MovingSphere.h"
#include "Sphere.h"
#include "Ray.h"
#include "Vec3.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <future>
#include <vector>

struct ScanlineResult
{
	int ScanlineIndex;
	std::vector<Colour> ScanlineData;
};

struct Settings
{
	int Width;
	float AspectRatio;
	int MaxDepth;
	int SamplesPerPixel;

	constexpr int Height() const { return static_cast<int>(Width / AspectRatio); }
};

Colour RayColour(const Ray& R, const BoundingVolumeHierarchy& world, int Depth)
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

ScanlineResult TraceScanline(const int Scanline, const Camera& Camera, const BoundingVolumeHierarchy& World, const Settings& Config)
{
	ScanlineResult result;
	result.ScanlineIndex = Scanline;

	for (int x = 0; x < Config.Width; x++)
	{
		Colour pixelColour{ 0.0f, 0.0f, 0.0f };
		for (int sample = 0; sample < Config.SamplesPerPixel; sample++)
		{
			const float u = (static_cast<float>(x) + Common::Random()) / (Config.Width - 1);
			const float v = (static_cast<float>(Scanline) + Common::Random()) / (Config.Height() - 1);
			Ray ray = Camera.GetRay(u, v);
			pixelColour += RayColour(ray, World, Config.MaxDepth);
		}

		result.ScanlineData.push_back(pixelColour);
	}

	return result;
}

BoundingVolumeHierarchy CoverScene()
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
					Point3 centre2 = centre + Point3(0.0f, Common::Random(0.0f, 0.5f), 0.0f);
					world.Add(std::make_shared<MovingSphere>(centre, centre2, 0.0f, 1.0f, 0.2f, mat));
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

	return BoundingVolumeHierarchy(world, 0.0f, 1.0f);
}

int main(int argc, char** argv)
{
	using Clock = std::chrono::high_resolution_clock;
	const auto startTime = Clock::now();

	constexpr Settings settings{ 1200, 16.0f / 9.0f, 500, 500 };

	const BoundingVolumeHierarchy world = CoverScene();

	const Point3 lookFrom = Point3(13.0f, 2.0f, 3.0f);
	const Point3 lookAt = Point3(0.0f, 0.0f, 0.0f);
	const Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
	const float focalDistance = 10.0f;
	const float aperture = 0.1f;
	const float fov = 20.0f;

	Camera camera(lookFrom, lookAt, up, fov, settings.AspectRatio, aperture, focalDistance, 0.0f, 1.0f);

	std::cout << "P3\n" << settings.Width << ' ' << settings.Height() << "\n255\n";

	constexpr int ConcurrentThreadCount = 64;
	std::future<ScanlineResult> jobs[ConcurrentThreadCount];
	const auto finishedSetup = Clock::now();

	int scanLineIdx = settings.Height() - 1;
	while (scanLineIdx > 0)
	{
		int runningThreads = 0;
		for (int threadIdx = 0; threadIdx < ConcurrentThreadCount; threadIdx++)
		{
			if (scanLineIdx > 0)
			{
				jobs[threadIdx] = std::async(&TraceScanline, scanLineIdx, camera, world, settings);
				scanLineIdx--;
				runningThreads++;
			}
		}

		for (int threadIdx = 0; threadIdx < runningThreads; threadIdx++)
		{
			ScanlineResult result = jobs[threadIdx].get();
			for (const Colour& colour : result.ScanlineData)
			{
				WriteColour(std::cout, colour, settings.SamplesPerPixel);
			}
		}

		std::cerr << "\rScanlines remaining: " << scanLineIdx << ' ' << std::flush;
	}

	const auto finishedRender = Clock::now();

	using DurationUnit = std::chrono::duration<float>;
	const DurationUnit setupDuration = std::chrono::duration_cast<DurationUnit>(finishedSetup - startTime);
	const DurationUnit renderDuration = std::chrono::duration_cast<DurationUnit>(finishedRender - finishedSetup);
	const DurationUnit totalDuration = std::chrono::duration_cast<DurationUnit>(finishedRender - startTime);
	std::cerr << "\nDone.\nSetup time: " << setupDuration.count() << "s\nRender time: " << renderDuration.count() << "s\nTotal time: " << totalDuration.count() << "s";
}