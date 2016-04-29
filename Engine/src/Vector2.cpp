#include "MathUtil.h"
#include "Vector2.h"
#include "StringStream.h"

namespace Sentinel
{
	const Vector2 Vector2::ZERO = Vector2(0, 0);
	const Vector2 Vector2::ONE = Vector2(1, 1);

	////////////////////////////////

	Vector2::Vector2()
	{
		x = y = 0;
	}

	Vector2::Vector2(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	Vector2::Vector2(float* v)
	{
		x = v[0];
		y = v[1];
	}

	float Vector2::operator [] (int i) const
	{
		_ASSERT(i >= 0 && i < 2);

		return (&x)[i];
	}

	float& Vector2::operator [] (int i)
	{
		_ASSERT(i >= 0 && i < 2);

		return (&x)[i];
	}

	float* Vector2::Ptr() const
	{
		return const_cast<float*>(&x);
	}

	std::string Vector2::ToString() const
	{
		return STREAM("(" << x << ", " << y << ")");
	}

	Vector2 Vector2::operator - () const
	{
		return Vector2(-x, -y);
	}

	bool Vector2::operator == (const Vector2& v) const
	{
		return Equals(v);
	}

	bool Vector2::operator != (const Vector2& v) const
	{
		return !Equals(v);
	}

	bool Vector2::Equals(const Vector2& v) const
	{
		return (x == v.x && y == v.y);
	}

	Vector2& Vector2::operator += (const Vector2& v)
	{
		*this = Add(v);
		return *this;
	}

	Vector2 Vector2::operator + (const Vector2& v) const
	{
		return Add(v);
	}

	Vector2 Vector2::Add(const Vector2& v) const
	{
		return Vector2(x + v.x, y + v.y);
	}

	Vector2& Vector2::operator -= (const Vector2& v)
	{
		*this = Sub(v);
		return *this;
	}

	Vector2 Vector2::operator - (const Vector2& v) const
	{
		return Sub(v);
	}

	Vector2 Vector2::Sub(const Vector2& v) const
	{
		return Vector2(x - v.x, y - v.y);
	}

	Vector2& Vector2::operator *= (float scalar)
	{
		*this = Mul(scalar);
		return *this;
	}

	Vector2 Vector2::operator * (float scalar) const
	{
		return Mul(scalar);
	}

	Vector2 Vector2::Mul(float scalar) const
	{
		return Vector2(x*scalar, y*scalar);
	}

	Vector2& Vector2::operator *= (const Vector2& v)
	{
		*this = Mul(v);
		return *this;
	}

	Vector2 Vector2::operator * (const Vector2& v) const
	{
		return Mul(v);
	}

	Vector2 Vector2::Mul(const Vector2& v) const
	{
		return Vector2(x*v.x, y*v.y);
	}

	Vector2 Vector2::Min(const Vector2& v) const
	{
		return Vector2(std::fmin(x, v.x), std::fmin(y, v.y));
	}

	Vector2 Vector2::Max(const Vector2& v) const
	{
		return Vector2(std::fmax(x, v.x), std::fmax(y, v.y));
	}

	float Vector2::Length() const
	{
		return sqrt(LengthSquared());
	}

	float Vector2::LengthSquared() const
	{
		return x*x + y*y;
	}

	float Vector2::LengthManhattan() const
	{
		return abs(x) + abs(y);
	}

	Vector2 Vector2::Normalize() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(1.0f / sqrt(len));

		return Vector2();
	}

	Vector2 Vector2::NormalizeFast() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(invsqrt(len));

		return Vector2();
	}

	float Vector2::Dot(const Vector2& v) const
	{
		return x*v.x + y*v.y;
	}

	Vector2 Vector2::Cross() const
	{
		return Vector2(-y, x);
	}

	float Vector2::Angle(const Vector2& v) const
	{
		return atan2(x*v.y - y*v.x, Dot(v));
	}
}
