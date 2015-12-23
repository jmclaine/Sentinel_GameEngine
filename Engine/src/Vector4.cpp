#include "MathUtil.h"
#include "Vector4.h"
#include "StringStream.h"

namespace Sentinel
{
	const Vector4 Vector4::ZERO = Vector4(0, 0, 0, 0);
	const Vector4 Vector4::ONE = Vector4(1, 1, 1, 1);

	////////////////////////////////

	Vector4::Vector4()
	{
		x = y = z = w = 0;
	}

	Vector4::Vector4(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	Vector4::Vector4(float* v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
		w = v[3];
	}

	float Vector4::operator [] (int i) const
	{
		_ASSERT(i >= 0 && i < 4);

		return (&x)[i];
	}

	float& Vector4::operator [] (int i)
	{
		_ASSERT(i >= 0 && i < 4);

		return (&x)[i];
	}

	float* Vector4::Ptr() const
	{
		return const_cast<float*>(&x);
	}

	std::string Vector4::ToString() const
	{
		return STREAM("(" << x << ", " << y << ", " << z << ", " << w << ")");
	}

	Vector4 Vector4::operator - () const
	{
		return Vector4(-x, -y, -z, -w);
	}

	bool Vector4::operator == (const Vector4& v) const
	{
		return Equals(v);
	}

	bool Vector4::operator != (const Vector4& v) const
	{
		return !Equals(v);
	}

	bool Vector4::Equals(const Vector4& v) const
	{
		return (x == v.x && y == v.y && z == v.z && w == v.w);
	}

	Vector4& Vector4::operator += (const Vector4& v)
	{
		*this = Add(v);
		return *this;
	}

	Vector4 Vector4::operator + (const Vector4& v) const
	{
		return Add(v);
	}

	Vector4 Vector4::Add(const Vector4& v) const
	{
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vector4& Vector4::operator -= (const Vector4& v)
	{
		*this = Sub(v);
		return *this;
	}

	Vector4 Vector4::operator - (const Vector4& v) const
	{
		return Sub(v);
	}

	Vector4 Vector4::Sub(const Vector4& v) const
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4& Vector4::operator *= (float scalar)
	{
		*this = Mul(scalar);
		return *this;
	}

	Vector4 Vector4::operator * (float scalar) const
	{
		return Mul(scalar);
	}

	Vector4 Vector4::Mul(const float scalar) const
	{
		return Vector4(x*scalar, y*scalar, z*scalar, w*scalar);
	}

	Vector4& Vector4::operator *= (const Vector4& v)
	{
		*this = Mul(v);
		return *this;
	}

	Vector4 Vector4::operator * (const Vector4& v) const
	{
		return Mul(v);
	}

	Vector4 Vector4::Mul(const Vector4& v) const
	{
		return Vector4(x*v.x, y*v.y, z*v.z, w*v.w);
	}

	Vector4 Vector4::Min(const Vector4& v) const
	{
		return Vector4(MIN(x, v.x), MIN(y, v.y), MIN(z, v.z), MIN(w, v.w));
	}

	Vector4 Vector4::Max(const Vector4& v) const
	{
		return Vector4(MAX(x, v.x), MAX(y, v.y), MAX(z, v.z), MAX(w, v.w));
	}

	float Vector4::Length() const
	{
		return sqrt(LengthSquared());
	}

	float Vector4::LengthSquared() const
	{
		return x*x + y*y + z*z + w*w;
	}

	float Vector4::LengthManhattan() const
	{
		return abs(x) + abs(y) + abs(z) + abs(w);
	}

	Vector4 Vector4::Normalize() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(1.0f / sqrt(len));

		return Vector4();
	}

	Vector4 Vector4::NormalizeFast() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(invsqrt(len));

		return Vector4();
	}

	float Vector4::Dot(const Vector4& v) const
	{
		return x*v.x + y*v.y + z*v.z + w*v.w;
	}

	Vector4 Vector4::Cross(const Vector4& v) const
	{
		return Vector4(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x, 0.0f);
	}

	float Vector4::Angle(const Vector4& v) const
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
			Vector4 sign = Cross(v);

			if (Vector4(0, 1, 0, 0).Dot(sign) < 0)
				a = -a;
		}

		return a;
	}
}
