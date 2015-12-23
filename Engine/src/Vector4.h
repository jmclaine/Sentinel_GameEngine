#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	class SENTINEL_DLL Vector4
	{
	public:

		static const Vector4 ZERO;
		static const Vector4 ONE;

		////////////////////////////////

		float x, y, z, w;

		////////////////////////////////

		Vector4();
		Vector4(float _x, float _y, float _z, float _w);
		Vector4(float* _v);

		float operator [] (int i) const;
		float& operator [] (int i);

		float* Ptr() const;

		std::string ToString() const;

		Vector4 operator - () const;

		bool operator == (const Vector4& v) const;
		bool operator != (const Vector4& v) const;
		bool Equals(const Vector4& v) const;

		Vector4& operator += (const Vector4& v);
		Vector4 operator +  (const Vector4& v) const;
		Vector4 Add(const Vector4& v) const;

		Vector4& operator -= (const Vector4& v);
		Vector4 operator -  (const Vector4& v) const;
		Vector4 Sub(const Vector4& v) const;

		Vector4& operator *= (float scalar);
		Vector4 operator *  (float scalar) const;
		Vector4 Mul(float scalar) const;

		Vector4& operator *= (const Vector4& v);
		Vector4 operator *  (const Vector4& v) const;
		Vector4 Mul(const Vector4& v) const;

		Vector4 Min(const Vector4& v) const;
		Vector4 Max(const Vector4& v) const;

		float Length() const;
		float LengthSquared() const;
		float LengthManhattan() const;

		Vector4 Normalize() const;
		Vector4 NormalizeFast() const;

		float Dot(const Vector4& v) const;
		Vector4 Cross(const Vector4& v) const;

		float Angle(const Vector4& v) const;
	};
}
