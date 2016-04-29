#include "MathUtil.h"
#include "Vector3.h"
#include "StringStream.h"

namespace Sentinel
{
	const Vector3 Vector3::ZERO = Vector3(0, 0, 0);
	const Vector3 Vector3::ONE = Vector3(1, 1, 1);

	////////////////////////////////

	Vector3::Vector3()
	{
		x = y = z = 0;
	}

	Vector3::Vector3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3::Vector3(float* v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	float Vector3::operator [] (int i) const
	{
		_ASSERT(i >= 0 && i < 3);

		return (&x)[i];
	}

	float& Vector3::operator [] (int i)
	{
		_ASSERT(i >= 0 && i < 3);

		return (&x)[i];
	}

	float* Vector3::Ptr() const
	{
		return const_cast<float*>(&x);
	}

	std::string Vector3::ToString() const
	{
		return STREAM("(" << x << ", " << y << ", " << z << ")");
	}

	Vector3 Vector3::operator - () const
	{
		return Vector3(-x, -y, -z);
	}

	bool Vector3::operator == (const Vector3& v) const
	{
		return Equals(v);
	}

	bool Vector3::operator != (const Vector3& v) const
	{
		return !Equals(v);
	}

	bool Vector3::Equals(const Vector3& v) const
	{
		return (x == v.x && y == v.y && z == v.z);
	}

	Vector3& Vector3::operator += (const Vector3& v)
	{
		return *this = Add(v);
	}

	Vector3 Vector3::operator + (const Vector3& v) const
	{
		return Add(v);
	}

	Vector3 Vector3::Add(const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3& Vector3::operator -= (const Vector3& v)
	{
		*this = Sub(v);
		return *this;
	}

	Vector3 Vector3::operator - (const Vector3& v) const
	{
		return Sub(v);
	}

	Vector3 Vector3::Sub(const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3& Vector3::operator *= (float scalar)
	{
		*this = Mul(scalar);
		return *this;
	}

	Vector3 Vector3::operator * (float scalar) const
	{
		return Mul(scalar);
	}

	Vector3 Vector3::Mul(float scalar) const
	{
		return Vector3(x*scalar, y*scalar, z*scalar);
	}

	Vector3& Vector3::operator *= (const Vector3& v)
	{
		*this = Mul(v);
		return *this;
	}

	Vector3 Vector3::operator * (const Vector3& v) const
	{
		return Mul(v);
	}

	Vector3 Vector3::Mul(const Vector3& v) const
	{
		return Vector3(x*v.x, y*v.y, z*v.z);
	}

	Vector3& Vector3::operator /= (float scalar)
	{
		*this = Div(scalar);
		return *this;
	}

	Vector3 Vector3::operator / (float scalar) const
	{
		return Div(scalar);
	}

	Vector3 Vector3::Div(float scalar) const
	{
		return Vector3(x/scalar, y/scalar, z/scalar);
	}

	Vector3& Vector3::operator /= (const Vector3& v)
	{
		*this = Div(v);
		return *this;
	}

	Vector3 Vector3::operator / (const Vector3& v) const
	{
		return Div(v);
	}

	Vector3 Vector3::Div(const Vector3& v) const
	{
		return Vector3(x/v.x, y/v.y, z/v.z);
	}

	Vector3 Vector3::Min(const Vector3& v) const
	{
		return Vector3(std::fmin(x, v.x), std::fmin(y, v.y), std::fmin(z, v.z));
	}

	Vector3 Vector3::Max(const Vector3& v) const
	{
		return Vector3(std::fmax(x, v.x), std::fmax(y, v.y), std::fmax(z, v.z));
	}

	float Vector3::Length() const
	{
		return sqrt(LengthSquared());
	}

	float Vector3::LengthSquared() const
	{
		return x*x + y*y + z*z;
	}

	float Vector3::LengthManhattan() const
	{
		return abs(x) + abs(y) + abs(z);
	}

	Vector3 Vector3::Normalize() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(1.0f / sqrt(len));

		return Vector3();
	}

	Vector3 Vector3::NormalizeFast() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(invsqrt(len));

		return Vector3();
	}

	float Vector3::Dot(const Vector3& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}

	Vector3 Vector3::Cross(const Vector3& v) const
	{
		return Vector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}

	float Vector3::Angle(const Vector3& v) const
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
			Vector3 sign = Cross(v);

			if (Vector3(0, 1, 0).Dot(sign) < 0)
				a = -a;
		}

		return a;
	}
}
