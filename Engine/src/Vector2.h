#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	class SENTINEL_DLL Vector2
	{
	public:

		static const Vector2 ZERO;
		static const Vector2 ONE;

		////////////////////////////////

		float x, y;

		////////////////////////////////

		Vector2();
		Vector2(float _x, float _y);
		Vector2(float* _v);

		float operator [] (int i) const;
		float& operator [] (int i);

		float* Ptr() const;

		std::string ToString() const;

		Vector2 operator - () const;

		bool operator == (const Vector2& v) const;
		bool operator != (const Vector2& v) const;
		bool Equals(const Vector2& v) const;

		Vector2& operator += (const Vector2& v);
		Vector2 operator +  (const Vector2& v) const;
		Vector2 Add(const Vector2& v) const;

		Vector2& operator -= (const Vector2& v);
		Vector2 operator -  (const Vector2& v) const;
		Vector2 Sub(const Vector2& v) const;

		Vector2& operator *= (float scalar);
		Vector2 operator *  (float scalar) const;
		Vector2 Mul(float scalar) const;

		Vector2& operator *= (const Vector2& v);
		Vector2 operator *  (const Vector2& v) const;
		Vector2 Mul(const Vector2& v) const;

		Vector2 Min(const Vector2& v) const;
		Vector2 Max(const Vector2& v) const;

		float Length() const;
		float LengthSquared() const;
		float LengthManhattan() const;

		Vector2 Normalize() const;
		Vector2 NormalizeFast() const;

		float Dot(const Vector2& v) const;
		Vector2 Cross() const;

		float Angle(const Vector2& v) const;
	};
}
