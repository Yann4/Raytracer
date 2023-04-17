#include "AARect.h"
#include "BoundingVolumeHierarchy.h"
#include "Box.h"
#include "Camera.h"
#include "Common.h"
#include "ConstantMedium.h"
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

enum class Scene { Cover, Nuts, Noise, Earth, LightSimple, Cornell, SmokeCornell, Final };

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
	Scene SelectedScene;
	Colour Background;

	constexpr int Height() const { return static_cast<int>(Width / AspectRatio); }
};

Colour RayColour(const Ray& R, const Colour& Background, const BoundingVolumeHierarchy& world, int Depth)
{
	if (Depth <= 0)
	{
		return Colour{ 0.0f };
	}

	HitRecord hit;
	if (!world.Hit(R, 0.001f, Common::Infinity, hit))
	{
		return Background;
	}

	Ray scattered;
	Colour attenuation;
	Colour emitted = hit.HitMaterial->Emit(hit.U, hit.V, hit.Position);

	if (!hit.HitMaterial->Scatter(R, hit, attenuation, scattered))
	{
		return emitted;
	}

	return emitted + attenuation * RayColour(scattered, Background, world, Depth - 1);
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
			pixelColour += RayColour(ray, Config.Background, World, Config.MaxDepth);
		}

		result.ScanlineData.push_back(pixelColour);
	}

	return result;
}

BoundingVolumeHierarchy CoverScene()
{
	HittableList world;

	std::shared_ptr<Texture> checkerTexture = std::make_shared<CheckerTexture>(Colour(0.2f, 0.3f, 0.1f), Colour(0.9f));
	world.Add(std::make_shared<Sphere>(Point3(0.0f, -1000.0f, 0.0f), 1000.0f, std::make_shared<Lambertian>(checkerTexture)));

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

BoundingVolumeHierarchy TwoSpheres()
{
	HittableList objects;

	std::shared_ptr<CheckerTexture> checker = std::make_shared<CheckerTexture>(Colour(0.2f, 0.3f, 0.1f), Colour(0.9f, 0.9f, 0.9f));

	objects.Add(std::make_shared<Sphere>(Point3(0.0f, -10.0f, 0.0f), 10.0f, std::make_shared<Lambertian>(checker)));
	objects.Add(std::make_shared<Sphere>(Point3(0.0f, 10.0f, 0.0f), 10.0f, std::make_shared<Lambertian>(checker)));

	return BoundingVolumeHierarchy(objects);
}

BoundingVolumeHierarchy TwoPerlinSpheres()
{
	HittableList objects;

	std::shared_ptr<NoiseTexture> noiseTexture = std::make_shared<NoiseTexture>(4.0f);

	objects.Add(std::make_shared<Sphere>(Point3(0.0f, -1000.0f, 0.0f), 1000.0f, std::make_shared<Lambertian>(noiseTexture)));
	objects.Add(std::make_shared<Sphere>(Point3(0.0f, 2.0f, 0.0f), 2.0f, std::make_shared<Lambertian>(noiseTexture)));

	return BoundingVolumeHierarchy(objects);
}

BoundingVolumeHierarchy Earth()
{
	HittableList objects;

	std::shared_ptr<Texture> earthTexture = std::make_shared<ImageTexture>("../../Assets/earthmap.jpg");
	std::shared_ptr<Material> earthMaterial = std::make_shared<Lambertian>(earthTexture);
	objects.Add(std::make_shared<Sphere>(Point3{ 0.0f }, 2.0f, earthMaterial));

	return BoundingVolumeHierarchy(objects);
}

BoundingVolumeHierarchy SimpleLight()
{
	HittableList objects;

	std::shared_ptr<Texture> perlinTexture = std::make_shared<NoiseTexture>(4.0f);
	objects.Add(std::make_shared<Sphere>(Point3(0.0f, -1000.0f, 0.0f), 1000.0f, std::make_shared<Lambertian>(perlinTexture)));
	objects.Add(std::make_shared<Sphere>(Point3(0.0f, 2.0f, 0.0f), 2.0f, std::make_shared<Lambertian>(perlinTexture)));

	std::shared_ptr<Material> light = std::make_shared<DiffuseLight>(Colour(4.0f));
	objects.Add(std::make_shared<XYRect>(3.0f, 5.0f, 1.0f, 3.0f, -2.0f, light));

	return BoundingVolumeHierarchy(objects);
}

BoundingVolumeHierarchy CornellBox()
{
	HittableList objects;

	std::shared_ptr<Material> red = std::make_shared<Lambertian>(Colour(0.65f, 0.05f, 0.05f));
	std::shared_ptr<Material> white = std::make_shared<Lambertian>(Colour(0.73f));
	std::shared_ptr<Material> green = std::make_shared<Lambertian>(Colour(0.12f, 0.45f, 0.15f));
	std::shared_ptr<Material> light = std::make_shared<DiffuseLight>(Colour(15.0f));

	objects.Add(std::make_shared<YZRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, green));
	objects.Add(std::make_shared<YZRect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, red));
	objects.Add(std::make_shared<XZRect>(213.0f, 343.0f, 227.0f, 332.0f, 554.0f, light));
	objects.Add(std::make_shared<XZRect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, white));
	objects.Add(std::make_shared<XZRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));
	objects.Add(std::make_shared<XYRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));

	std::shared_ptr<IHittable> box1 = std::make_shared<Box>(Point3(0.0f), Point3(165.0f, 330.0f, 165.0f), white);
	box1 = std::make_shared<RotateY>(box1, 15.0f);
	box1 = std::make_shared<Translate>(box1, Vec3(265.0f, 0.0f, 295.0f));
	objects.Add(box1);

	std::shared_ptr<IHittable> box2 = std::make_shared<Box>(Point3(0.0f), Point3(165.0f), white);
	box2 = std::make_shared<RotateY>(box2, -18.0f);
	box2 = std::make_shared<Translate>(box2, Vec3(130.0f, 0.0f, 65.0f));
	objects.Add(box2);

	return BoundingVolumeHierarchy(objects);
}

BoundingVolumeHierarchy CornellSmoke() 
{
	HittableList objects;

	std::shared_ptr<Material> red = std::make_shared<Lambertian>(Colour(0.65f, 0.05f, 0.05f));
	std::shared_ptr<Material> white = std::make_shared<Lambertian>(Colour(0.73f));
	std::shared_ptr<Material> green = std::make_shared<Lambertian>(Colour(0.12f, 0.45f, 0.15f));
	std::shared_ptr<Material> light = std::make_shared<DiffuseLight>(Colour(7.0f));

	objects.Add(std::make_shared<YZRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, green));
	objects.Add(std::make_shared<YZRect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, red));
	objects.Add(std::make_shared<XZRect>(113.0f, 443.0f, 127.0f, 432.0f, 554.0f, light));
	objects.Add(std::make_shared<XZRect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, white));
	objects.Add(std::make_shared<XZRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));
	objects.Add(std::make_shared<XYRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));

	std::shared_ptr<IHittable> box1 = std::make_shared<Box>(Point3(0.0f), Point3(165.0f, 330.0f, 165.0f), white);
	box1 = std::make_shared<RotateY>(box1, 15.0f);
	box1 = std::make_shared<Translate>(box1, Vec3(265.0f, 0.0f, 295.0f));

	std::shared_ptr<IHittable> box2 = std::make_shared<Box>(Point3(0.0f), Point3(165.0f), white);
	box2 = std::make_shared<RotateY>(box2, -18.0f);
	box2 = std::make_shared<Translate>(box2, Vec3(130.0f, 0.0f, 65.0f));

	objects.Add(std::make_shared<ConstantMedium>(box1, 0.01f, Colour(0.0f)));
	objects.Add(std::make_shared<ConstantMedium>(box2, 0.01f, Colour(1.0f)));

	return BoundingVolumeHierarchy(objects);
}

BoundingVolumeHierarchy FinalScene()
{
	HittableList boxes1;
	std::shared_ptr<Material> ground = std::make_shared<Lambertian>(Colour(0.48f, 0.83f, 0.53f));

	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0f;
			auto x0 = -1000.0f + i * w;
			auto z0 = -1000.0f + j * w;
			auto y0 = 0.0f;
			auto x1 = x0 + w;
			auto y1 = Common::Random(1.0f, 101.0f);
			auto z1 = z0 + w;

			boxes1.Add(std::make_shared<Box>(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
		}
	}

	HittableList objects;

	objects.Add(std::make_shared<BoundingVolumeHierarchy>(boxes1, 0.0f, 1.0f));

	std::shared_ptr<Material> light = std::make_shared<DiffuseLight>(Colour(7.0f));
	objects.Add(std::make_shared<XZRect>(123.0f, 423.0f, 147.0f, 412.0f, 554.0f, light));

	const Point3 center1 = Point3(400.0f, 400.0f, 200.0f);
	const Point3 center2 = center1 + Vec3(30.0f, 0.0f, 0.0f);
	std::shared_ptr<Material> movingSphereMat = std::make_shared<Lambertian>(Colour(0.7f, 0.3f, 0.1f));
	objects.Add(std::make_shared<MovingSphere>(center1, center2, 0.0f, 1.0f, 50.0f, movingSphereMat));

	objects.Add(std::make_shared<Sphere>(Point3(260.0f, 150.0f, 45.0f), 50.0f, std::make_shared<Dielectric>(1.5f)));
	objects.Add(std::make_shared<Sphere>(
		Point3(0.0f, 150.0f, 145.0f), 50.0f, std::make_shared<Metal>(Colour(0.8f, 0.8f, 0.9f), 1.0f)
		));

	std::shared_ptr<IHittable> boundary = std::make_shared<Sphere>(Point3(360.0f, 150.0f, 145.0f), 70.0f, std::make_shared<Dielectric>(1.5f));
	objects.Add(boundary);
	objects.Add(std::make_shared<ConstantMedium>(boundary, 0.2f, Colour(0.2f, 0.4f, 0.9f)));
	boundary = std::make_shared<Sphere>(Point3(0.0f), 5000.0f, std::make_shared<Dielectric>(1.5f));
	objects.Add(std::make_shared<ConstantMedium>(boundary, 0.0001f, Colour(1.0f)));

	std::shared_ptr<Material> emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("../../Assets/earthmap.jpg"));
	objects.Add(std::make_shared<Sphere>(Point3(400.0f, 200.0f, 400.0f), 100.0f, emat));
	std::shared_ptr<Texture> pertext = std::make_shared<NoiseTexture>(0.1f);
	objects.Add(std::make_shared<Sphere>(Point3(220.0f, 280.0f, 300.0f), 80.0f, std::make_shared<Lambertian>(pertext)));

	HittableList boxes2;
	std::shared_ptr<Material> white = std::make_shared<Lambertian>(Colour(0.73f));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.Add(std::make_shared<Sphere>(Point3::Random(0.0f, 165.0f), 10.0f, white));
	}

	objects.Add(std::make_shared<Translate>(
		std::make_shared<RotateY>(
			std::make_shared<BoundingVolumeHierarchy>(boxes2, 0.0f, 1.0f), 15.0f),
		Vec3(-100.0f, 270.0f, 395.0f)
		)
	);

	return BoundingVolumeHierarchy(objects);
}

int main(int argc, char** argv)
{
	using Clock = std::chrono::high_resolution_clock;
	const auto startTime = Clock::now();

	Settings settings{ 1200, 16.0f / 9.0f, 50, 100, Scene::Final, Colour{0.0f} };

	Point3 lookFrom;
	Point3 lookAt;
	Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
	float focalDistance = 10.0f;
	float aperture = 0.1f;
	float fov = 20.0f;

	BoundingVolumeHierarchy world;
	switch (settings.SelectedScene)
	{
	case Scene::Cover:
		world = CoverScene();
		lookFrom = Point3(13.0f, 2.0f, 3.0f);
		lookAt = Point3(0.0f, 0.0f, 0.0f);
		settings.Background = Colour(0.7f, 0.8f, 1.0f);
		break;
	case Scene::Nuts:
		world = TwoSpheres();
		lookFrom = Point3(13.0f, 2.0f, 3.0f);
		lookAt = Point3(0.0f, 0.0f, 0.0f);
		focalDistance = 10.0f;
		settings.Background = Colour(0.7f, 0.8f, 1.0f);
		break;
	case Scene::Noise:
		world = TwoPerlinSpheres();
		lookFrom = Point3(13.0f, 2.0f, 3.0f);
		lookAt = Point3(0.0f, 0.0f, 0.0f);
		settings.Background = Colour(0.7f, 0.8f, 1.0f);
		break;
	case Scene::Earth:
		world = Earth();
		lookFrom = Point3(13.0f, 2.0f, 3.0f);
		lookAt = Point3(0.0f, 0.0f, 0.0f);
		focalDistance = 10.0f;
		settings.Background = Colour(0.7f, 0.8f, 1.0f);
		break;
	case Scene::LightSimple:
		world = SimpleLight();
		lookFrom = Point3(26.0f, 3.0f, 6.0f);
		lookAt = Point3(0.0f, 3.0f, 0.0f);
		settings.Background = Colour(0.0f);
		settings.SamplesPerPixel = 400;
		break;
	case Scene::Cornell:
		world = CornellBox();
		settings.AspectRatio = 1.0;
		settings.Width = 600;
		settings.SamplesPerPixel = 200;
		settings.Background = Colour(0.0f);
		lookFrom = Point3(278.0f, 278.0f, -800.0f);
		lookAt = Point3(278.0f, 278.0f, 0.0f);
		fov = 40.0f;
		break;
	case Scene::SmokeCornell:
		world = CornellSmoke();
		settings.AspectRatio = 1.0;
		settings.Width = 600;
		settings.SamplesPerPixel = 200;
		settings.Background = Colour(0.0f);
		lookFrom = Point3(278.0f, 278.0f, -800.0f);
		lookAt = Point3(278.0f, 278.0f, 0.0f);
		fov = 40.0f;
		break;
	case Scene::Final:
		world = FinalScene();
		settings.AspectRatio = 1.0;
		settings.Width = 800;
		settings.SamplesPerPixel = 10000;
		settings.Background = Colour(0.0f);
		lookFrom = Point3(478.0f, 278.0f, -600.0f);
		lookAt = Point3(278.0f, 278.0f, 0.0f);
		fov = 40.0f;
		break;
	}

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