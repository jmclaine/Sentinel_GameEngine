#include "MathUtil.h"
#include "Vector4f.h"

namespace Sentinel
{
	const Vector4f Vector4f::ZERO = Vector4f(0, 0, 0, 0);
	const Vector4f Vector4f::ONE = Vector4f(1, 1, 1, 1);

	////////////////////////////////

	Vector4f::Vector4f()
	{
		x = y = z = w = 0;
	}

	Vector4f::Vector4f(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	Vector4f::Vector4f(float* v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
		w = v[3];
	}

	float Vector4f::operator [] (int i) const
	{
		_ASSERT(i >= 0 && i < 4);

		return (&x)[i];
	}

	float& Vector4f::operator [] (int i)
	{
		_ASSERT(i >= 0 && i < 4);

		return (&x)[i];
	}

	float* Vector4f::Ptr()
	{
		return &x;
	}

	Vector4f Vector4f::operator - () const
	{
		return Vector4f(-x, -y, -z, -w);
	}

	bool Vector4f::operator == (const Vector4f& v) const
	{
		return Equals(v);
	}

	bool Vector4f::operator != (const Vector4f& v) const
	{
		return !Equals(v);
	}

	bool Vector4f::Equals(const Vector4f& v) const
	{
		return (x == v.x && y == v.y && z == v.z && w == v.w);
	}

	Vector4f& Vector4f::operator += (const Vector4f& v)
	{
		*this = Add(v);
		return *this;
	}

	Vector4f Vector4f::operator + (const Vector4f& v) const
	{
		return Add(v);
	}

	Vector4f Vector4f::Add(const Vector4f& v) const
	{
		return Vector4f(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vector4f& Vector4f::operator -= (const Vector4f& v)
	{
		*this = Sub(v);
		return *this;
	}

	Vector4f Vector4f::operator - (const Vector4f& v) const
	{
		return Sub(v);
	}

	Vector4f Vector4f::Sub(const Vector4f& v) const
	{
		return Vector4f(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4f& Vector4f::operator *= (float scalar)
	{
		*this = Mul(scalar);
		return *this;
	}

	Vector4f Vector4f::operator * (float scalar) const
	{
		return Mul(scalar);
	}

	Vector4f Vector4f::Mul(const float scalar) const
	{
		return Vector4f(x*scalar, y*scalar, z*scalar, w*scalar);
	}

	Vector4f& Vector4f::operator *= (const Vector4f& v)
	{
		*this = Mul(v);
		return *this;
	}

	Vector4f Vector4f::operator * (const Vector4f& v) const
	{
		return Mul(v);
	}

	Vector4f Vector4f::Mul(const Vector4f& v) const
	{
		return Vector4f(x*v.x, y*v.y, z*v.z, w*v.w);
	}

	Vector4f Vector4f::Min(const Vector4f& v) const
	{
		return Vector4f(MIN(x, v.x), MIN(y, v.y), MIN(z, v.z), MIN(w, v.w));
	}

	Vector4f Vector4f::Max(const Vector4f& v) const
	{
		return Vector4f(MAX(x, v.x), MAX(y, v.y), MAX(z, v.z), MAX(w, v.w));
	}

	float Vector4f::Length() const
	{
		return sqrt(LengthSquared());
	}

	float Vector4f::LengthSquared() const
	{
		return x*x + y*y + z*z + w*w;
	}

	float Vector4f::LengthManhattan() const
	{
		return abs(x) + abs(y) + abs(z) + abs(w);
	}

	Vector4f Vector4f::Normalize() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(1.0f / sqrt(len));

		return Vector4f();
	}

	Vector4f Vector4f::NormalizeFast() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(invsqrt(len));

		return Vector4f();
	}

	float Vector4f::Dot(const Vector4f& v) const
	{
		return x*v.x + y*v.y + z*v.z + w*v.w;
	}

	Vector4f Vector4f::Cross(const Vector4f& v) const
	{
		return Vector4f(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x, 0.0f);
	}

	float Vector4f::Angle(const Vector4f& v) const
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
			Vector4f sign = Cross(v);

			if (Vector4f(0, 1, 0, 0).Dot(sign) < 0)
				a = -a;
		}

		return a;
	}
}
