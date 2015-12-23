#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	inline int RandomValue(int minValue, int maxValue)
	{
		if (maxValue > minValue)
			std::swap(maxValue, minValue);

		return minValue + rand() % (maxValue - minValue);
	}

	inline unsigned int RandomValue(unsigned int minValue, unsigned int maxValue)
	{
		if (maxValue > minValue)
			std::swap(maxValue, minValue);

		return minValue + rand() % (maxValue - minValue);
	}

	inline float RandomValue(float minValue, float maxValue)
	{
		if (maxValue > minValue)
			std::swap(maxValue, minValue);

		return minValue + (maxValue - minValue) * (rand() / static_cast<float>(RAND_MAX));
	}

	inline double RandomValue(double minValue, double maxValue)
	{
		if (maxValue > minValue)
			std::swap(maxValue, minValue);

		return minValue + (maxValue - minValue) * (rand() / static_cast<double>(RAND_MAX));
	}

	inline Vector2 RandomValue(const Vector2& minValue, const Vector2& maxValue)
	{
		return Vector2(
			RandomValue(minValue.x, maxValue.x), \
			RandomValue(minValue.y, maxValue.y));
	}

	inline Vector3 RandomValue(const Vector3& minValue, const Vector3& maxValue)
	{
		return Vector3(
			RandomValue(minValue.x, maxValue.x), \
			RandomValue(minValue.y, maxValue.y), \
			RandomValue(minValue.z, maxValue.z));
	}

	inline Vector4 RandomValue(const Vector4& minValue, const Vector4& maxValue)
	{
		return Vector4(
			RandomValue(minValue.x, maxValue.x), \
			RandomValue(minValue.y, maxValue.y), \
			RandomValue(minValue.z, maxValue.z),
			RandomValue(minValue.w, maxValue.w));
	}

	inline ColorRGBA RandomValue(const ColorRGBA& minColor, const ColorRGBA& maxColor)
	{
		return ColorRGBA(
			RandomValue(minColor.r, maxColor.r), \
			RandomValue(minColor.g, maxColor.g), \
			RandomValue(minColor.b, maxColor.b), \
			RandomValue(minColor.a, maxColor.a));
	}
}
