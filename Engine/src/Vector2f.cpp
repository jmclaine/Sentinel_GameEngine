#include "MathUtil.h"
#include "Vector2f.h"

namespace Sentinel
{
	const Vector2f Vector2f::ZERO = Vector2f(0, 0);
	const Vector2f Vector2f::ONE = Vector2f(1, 1);

	////////////////////////////////

	Vector2f::Vector2f()
	{
		x = y = 0;
	}

	Vector2f::Vector2f(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	Vector2f::Vector2f(float* v)
	{
		x = v[0];
		y = v[1];
	}

	float Vector2f::operator [] (int i) const
	{
		_ASSERT(i >= 0 && i < 2);

		return (&x)[i];
	}

	float& Vector2f::operator [] (int i)
	{
		_ASSERT(i >= 0 && i < 2);

		return (&x)[i];
	}

	float* Vector2f::Ptr()
	{
		return static_cast<float*>(&x);
	}

	Vector2f Vector2f::operator - () const
	{
		return Vector2f(-x, -y);
	}

	bool Vector2f::operator == (const Vector2f& v) const
	{
		return Equals(v);
	}

	bool Vector2f::operator != (const Vector2f& v) const
	{
		return !Equals(v);
	}

	bool Vector2f::Equals(const Vector2f& v) const
	{
		return (x == v.x && y == v.y);
	}

	Vector2f& Vector2f::operator += (const Vector2f& v)
	{
		*this = Add(v);
		return *this;
	}

	Vector2f Vector2f::operator + (const Vector2f& v) const
	{
		return Add(v);
	}

	Vector2f Vector2f::Add(const Vector2f& v) const
	{
		return Vector2f(x + v.x, y + v.y);
	}

	Vector2f& Vector2f::operator -= (const Vector2f& v)
	{
		*this = Sub(v);
		return *this;
	}

	Vector2f Vector2f::operator - (const Vector2f& v) const
	{
		return Sub(v);
	}

	Vector2f Vector2f::Sub(const Vector2f& v) const
	{
		return Vector2f(x - v.x, y - v.y);
	}

	Vector2f& Vector2f::operator *= (float scalar)
	{
		*this = Mul(scalar);
		return *this;
	}

	Vector2f Vector2f::operator * (float scalar) const
	{
		return Mul(scalar);
	}

	Vector2f Vector2f::Mul(float scalar) const
	{
		return Vector2f(x*scalar, y*scalar);
	}

	Vector2f& Vector2f::operator *= (const Vector2f& v)
	{
		*this = Mul(v);
		return *this;
	}

	Vector2f Vector2f::operator * (const Vector2f& v) const
	{
		return Mul(v);
	}

	Vector2f Vector2f::Mul(const Vector2f& v) const
	{
		return Vector2f(x*v.x, y*v.y);
	}

	Vector2f Vector2f::Min(const Vector2f& v) const
	{
		return Vector2f(MIN(x, v.x), MIN(y, v.y));
	}

	Vector2f Vector2f::Max(const Vector2f& v) const
	{
		return Vector2f(MAX(x, v.x), MAX(y, v.y));
	}

	float Vector2f::Length() const
	{
		return sqrt(LengthSquared());
	}

	float Vector2f::LengthSquared() const
	{
		return x*x + y*y;
	}

	float Vector2f::LengthManhattan() const
	{
		return abs(x) + abs(y);
	}

	Vector2f Vector2f::Normalize() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(1.0f / sqrt(len));

		return Vector2f();
	}

	Vector2f Vector2f::NormalizeFast() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(invsqrt(len));

		return Vector2f();
	}

	float Vector2f::Dot(const Vector2f& v) const
	{
		return x*v.x + y*v.y;
	}

	Vector2f Vector2f::Cross() const
	{
		return Vector2f(-y, x);
	}

	float Vector2f::Angle(const Vector2f& v) const
	{
		return atan2(x*v.y - y*v.x, Dot(v));
	}
}
