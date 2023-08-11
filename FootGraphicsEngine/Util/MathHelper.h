#pragma once
#include <random>

namespace GraphicsEngineSpace
{
	/**
 * \brief 역전치 행렬 계산 등의 수학 정리가 들어가 있는 용책의 mathHelper..
 */
	class MathHelper
	{
	public:
		// Returns random float in [0, 1).
		static float RandF()
		{
			return (float)(rand()) / (float)RAND_MAX;
		}

		// Returns random float in [a, b).
		static float RandF(float a, float b)
		{
			static std::random_device rd;

			static std::mt19937 gen(rd());

			std::uniform_real_distribution floatDis(a, b);

			return floatDis(gen);
		}

		static float SmoothStep(float x, float min = 0.f, float max = 1.f)
		{
			x = std::clamp((x - min) / (max - min), 0.f, 1.f);

			return x * x * (3 - 2 * x);
		}

		template<typename T>
		static T Min(const T& a, const T& b)
		{
			return a < b ? a : b;
		}

		template<typename T>
		static T Max(const T& a, const T& b)
		{
			return a > b ? a : b;
		}

		template<typename T>
		static T Lerp(const T& a, const T& b, float t)
		{
			return a + (b - a) * t;
		}

		template<typename T>
		static T Clamp(const T& x, const T& low, const T& high)
		{
			return x < low ? low : (x > high ? high : x);
		}

		// Returns the polar angle of the point (x,y) in [0, 2*PI).
		static float AngleFromXY(float x, float y);

		static XMMATRIX InverseTranspose(CXMMATRIX M)
		{
			// Inverse-transpose is just applied to normals.  So zero out 
			// translation row so that it doesn't get into our inverse-transpose
			// calculation--we don't want the inverse-transpose of the translation.
			XMMATRIX A = M;
			A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMVECTOR det = XMMatrixDeterminant(A);
			return XMMatrixTranspose(XMMatrixInverse(&det, A));
		}

		static XMVECTOR RandUnitVec3();
		static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);

		static const float Infinity;
		static const float Pi;
	};

}
