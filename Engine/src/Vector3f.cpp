#include "MathUtil.h"
#include "Vector3f.h"
#include "StringStream.h"

namespace Sentinel
{
	const Vector3f Vector3f::ZERO = Vector3f(0, 0, 0);
	const Vector3f Vector3f::ONE = Vector3f(1, 1, 1);

	////////////////////////////////

	Vector3f::Vector3f()
	{
		x = y = z = 0;
	}

	Vector3f::Vector3f(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f::Vector3f(float* v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	float Vector3f::operator [] (int i) const
	{
		_ASSERT(i >= 0 && i < 3);

		return (&x)[i];
	}

	float& Vector3f::operator [] (int i)
	{
		_ASSERT(i >= 0 && i < 3);

		return (&x)[i];
	}

	float* Vector3f::Ptr() const
	{
		return const_cast<float*>(&x);
	}

	std::string Vector3f::ToString() const
	{
		return STREAM("(" << x << ", " << y << ", " << z << ")");
	}

	Vector3f Vector3f::operator - () const
	{
		return Vector3f(-x, -y, -z);
	}

	bool Vector3f::operator == (const Vector3f& v) const
	{
		return Equals(v);
	}

	bool Vector3f::operator != (const Vector3f& v) const
	{
		return !Equals(v);
	}

	bool Vector3f::Equals(const Vector3f& v) const
	{
		return (x == v.x && y == v.y && z == v.z);
	}

	Vector3f& Vector3f::operator += (const Vector3f& v)
	{
		return *this = Add(v);
	}

	Vector3f Vector3f::operator + (const Vector3f& v) const
	{
		return Add(v);
	}

	Vector3f Vector3f::Add(const Vector3f& v) const
	{
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f& Vector3f::operator -= (const Vector3f& v)
	{
		*this = Sub(v);
		return *this;
	}

	Vector3f Vector3f::operator - (const Vector3f& v) const
	{
		return Sub(v);
	}

	Vector3f Vector3f::Sub(const Vector3f& v) const
	{
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f& Vector3f::operator *= (float scalar)
	{
		*this = Mul(scalar);
		return *this;
	}

	Vector3f Vector3f::operator * (float scalar) const
	{
		return Mul(scalar);
	}

	Vector3f Vector3f::Mul(float scalar) const
	{
		return Vector3f(x*scalar, y*scalar, z*scalar);
	}

	Vector3f& Vector3f::operator *= (const Vector3f& v)
	{
		*this = Mul(v);
		return *this;
	}

	Vector3f Vector3f::operator * (const Vector3f& v) const
	{
		return Mul(v);
	}

	Vector3f Vector3f::Mul(const Vector3f& v) const
	{
		return Vector3f(x*v.x, y*v.y, z*v.z);
	}

	Vector3f Vector3f::Min(const Vector3f& v) const
	{
		return Vector3f(MIN(x, v.x), MIN(y, v.y), MIN(z, v.z));
	}

	Vector3f Vector3f::Max(const Vector3f& v) const
	{
		return Vector3f(MAX(x, v.x), MAX(y, v.y), MAX(z, v.z));
	}

	float Vector3f::Length() const
	{
		return sqrt(LengthSquared());
	}

	float Vector3f::LengthSquared() const
	{
		return x*x + y*y + z*z;
	}

	float Vector3f::LengthManhattan() const
	{
		return abs(x) + abs(y) + abs(z);
	}

	Vector3f Vector3f::Normalize() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(1.0f / sqrt(len));

		return Vector3f();
	}

	Vector3f Vector3f::NormalizeFast() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(invsqrt(len));

		return Vector3f();
	}

	float Vector3f::Dot(const Vector3f& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}

	Vector3f Vector3f::Cross(const Vector3f& v) const
	{
		return Vector3f(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}

	float Vector3f::Angle(const Vector3f& v) const
	{
		float len0 = LengthSquared();
		float len1 = v.LengthSquared();
		float a = 0;

		if (len0 > 0 && len1 > 0)
		{
			float d = Mul(1.0f / sqrt(len0)).Dot(v.Mul(1.0f / sqrt(len1)));

			// Prevent floating point errors.
			//
			if (d < -1)
			{
				d = -1;
			}
			else if (d > 1)
			{
				d = 1;
			}
			a = acos(d);

			// Determine sign.
			// Assume up is 0, 1, 0
			//
			Vector3f sign = Cross(v);

			if (Vector3f(0, 1, 0).Dot(sign) < 0)
				a = -a;
		}

		return a;
	}
}
