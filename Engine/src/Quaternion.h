#pragma once

#include "Sentinel.h"
#include "Matrix4x4.h"
#include "Vector3.h"

namespace Sentinel
{
	class SENTINEL_DLL Quaternion
	{
		friend class Matrix4x4;

	public:

		static const Quaternion IDENTITY;

		////////////////////////////////

		float x, y, z, w;

		////////////////////////////////

		Quaternion();
		Quaternion(float _pitch, float _yaw, float _roll);
		Quaternion(const Vector3& rotation);
		Quaternion(float _x, float _y, float _z, float _w);

		float* Ptr() const;

		std::string ToString() const;

		Quaternion operator - () const;

		bool operator == (const Quaternion& v) const;
		bool operator != (const Quaternion& v) const;
		bool Equals(const Quaternion& v) const;

		Quaternion& operator *= (const Quaternion& q);
		Quaternion operator * (const Quaternion& q) const;
		Quaternion Mul(const Quaternion& q) const;
		Quaternion Mul(float scalar) const;

		float Length() const;
		float LengthSquared() const;
		float LengthManhattan() const;

		Quaternion Normalize() const;
		Quaternion NormalizeFast() const;

		float Dot(const Quaternion& q) const;

		Quaternion Inverse() const;

		Quaternion& AxisAngle(float _x, float _y, float _z, float _degrees);
		Quaternion& AxisAngle();

		Quaternion& Euler(float _pitch, float _yaw, float _roll);
		Quaternion& Euler();
		Vector3 ToEuler();

		Vector3 Transform(const Vector3& v) const;

		Quaternion Slerp(const Quaternion& q, float t);

		Vector3 Forward() const;
		Vector3 Up() const;
		Vector3 Right() const;
	};
}
