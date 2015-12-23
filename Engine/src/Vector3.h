#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	class SENTINEL_DLL Vector3
	{
		friend class Matrix4x4;

	public:

		static const Vector3 ZERO;
		static const Vector3 ONE;

		////////////////////////////////

		float x, y, z;

		////////////////////////////////

		Vector3();
		Vector3(float _x, float _y, float _z);
		Vector3(float* v);

		float operator [] (int i) const;
		float& operator [] (int i);

		float* Ptr() const;

		std::string ToString() const;

		Vector3 operator - () const;

		bool operator == (const Vector3& v) const;
		bool operator != (const Vector3& v) const;
		bool Equals(const Vector3& v) const;

		Vector3& operator += (const Vector3& v);
		Vector3 operator + (const Vector3& v) const;
		Vector3 Add(const Vector3& v) const;

		Vector3& operator -= (const Vector3& v);
		Vector3 operator - (const Vector3& v) const;
		Vector3 Sub(const Vector3& v) const;

		Vector3& operator *= (float scalar);
		Vector3 operator * (float scalar) const;
		Vector3 Mul(float scalar) const;

		Vector3& operator *= (const Vector3& v);
		Vector3 operator * (const Vector3& v) const;
		Vector3 Mul(const Vector3& v) const;

		Vector3& operator /= (float scalar);
		Vector3 operator / (float scalar) const;
		Vector3 Div(float scalar) const;

		Vector3& operator /= (const Vector3& v);
		Vector3 operator / (const Vector3& v) const;
		Vector3 Div(const Vector3& v) const;

		Vector3 Min(const Vector3& v) const;
		Vector3 Max(const Vector3& v) const;

		float Length() const;
		float LengthSquared() const;
		float LengthManhattan() const;

		Vector3 Normalize() const;
		Vector3 NormalizeFast() const;

		float Dot(const Vector3& v) const;
		Vector3 Cross(const Vector3& v) const;

		float Angle(const Vector3& v) const;
	};
}
