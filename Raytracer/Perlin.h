#pragma once

#include "Common.h"
#include "Vec3.h"

class Perlin
{
public:
	Perlin()
	{
		m_RandVec = new Vec3[m_PointCount];
		for (int idx = 0; idx < m_PointCount; idx++)
		{
			m_RandVec[idx] = Vec3::Random(-1.0f, 1.0f);
		}

		m_PermX = GeneratePerm();
		m_PermY = GeneratePerm();
		m_PermZ = GeneratePerm();
	}

	~Perlin()
	{
		delete[] m_RandVec;
		delete[] m_PermX;
		delete[] m_PermY;
		delete[] m_PermZ;
	}

	float Noise(const Point3& P) const
	{
		const float u = P.x() - std::floor(P.x());
		const float v = P.y() - std::floor(P.y());
		const float w = P.z() - std::floor(P.z());

		const int i = static_cast<int>(std::floor(P.x()));
		const int j = static_cast<int>(std::floor(P.y()));
		const int k = static_cast<int>(std::floor(P.z()));
		Vec3 c[2][2][2];
		for(int di = 0; di < 2; di++)
		{
			for (int dj = 0; dj < 2; dj++)
			{
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = m_RandVec[m_PermX[(i + di) & 255] ^ m_PermY[(j + dj) & 255] ^ m_PermZ[(k + dk) & 255]];
				}
			}
		}

		return TrilinearInterp(c, u, v, w);
	}

	float Turbulence(const Point3& P, int Depth = 7) const
	{
		float accum = 0.0f;
		Point3 tempP = P;
		float weight = 1.0f;

		for (int i = 0; i < Depth; i++)
		{
			accum += weight * Noise(tempP);
			weight *= 0.5f;
			tempP *= 2;
		}

		return std::fabsf(accum);
	}
private:
	static int* GeneratePerm()
	{
		int* p = new int[m_PointCount];

		for (int idx = 0; idx < Perlin::m_PointCount; idx++)
		{
			p[idx] = idx;
		}

		Permute(p, m_PointCount);
		return p;
	}

	static void Permute(int* P, const int n)
	{
		for (int i = n - 1; i > 0; i--)
		{
			const int target = Common::RandomInt(0, i);
			const int tmp = P[i];
			P[i] = P[target];
			P[target] = tmp;
		}
	}

	static float TrilinearInterp(const Vec3 C[2][2][2], const float U, const float V, const float W)
	{
		auto uu = U * U * (3.0f - 2.0f * U);
		auto vv = V * V * (3.0f - 2.0f * V);
		auto ww = W * W * (3.0f - 2.0f * W);
		float accum = 0.0f;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					Vec3 weightV(U - i, V - j, W - k);
					accum += (i * uu + (1 - i) * (1 - uu))
						* (j * vv + (1 - j) * (1 - vv))
						* (k * ww + (1 - k) * (1 - ww))
						* Dot(C[i][j][k], weightV);
				}
			}
		}

		return accum;
	}
private:
	static constexpr int m_PointCount = 256;
	Vec3* m_RandVec;
	int* m_PermX;
	int* m_PermY;
	int* m_PermZ;
};