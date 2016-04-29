#include "MathUtil.h"
#include "Quaternion.h"
#include "StringStream.h"

namespace Sentinel
{
	const Quaternion Quaternion::IDENTITY = Quaternion(0, 0, 0, 1);

	////////////////////////////////

	Quaternion::Quaternion() : 
		x(0), y(0), z(0), w(1)
	{ }

	Quaternion::Quaternion(float _pitch, float _yaw, float _roll)
	{
		Euler(_pitch, _yaw, _roll);
	}

	Quaternion::Quaternion(const Vector3& rotation)
	{
		Euler(rotation.x, rotation.y, rotation.z);
	}

	Quaternion::Quaternion(float _x, float _y, float _z, float _w) :
		x(_x), y(_y), z(_z), w(_w)
	{ }

	float* Quaternion::Ptr() const
	{
		return const_cast<float*>(&x);
	}

	std::string Quaternion::ToString() const
	{
		return STREAM("(" << x << ", " << y << ", " << z << ", " << w << ")");
	}

	Quaternion Quaternion::operator - () const
	{
		return Inverse();
	}

	bool Quaternion::operator == (const Quaternion& v) const
	{
		return Equals(v);
	}

	bool Quaternion::operator != (const Quaternion& v) const
	{
		return !Equals(v);
	}

	bool Quaternion::Equals(const Quaternion& v) const
	{
		return (x == v.x && y == v.y && z == v.z && w == v.w);
	}

	Quaternion& Quaternion::operator *= (const Quaternion& q)
	{
		*this = Mul(q);
		return *this;
	}

	Quaternion Quaternion::operator * (const Quaternion& q) const
	{
		return Mul(q);
	}

	Quaternion Quaternion::Mul(const Quaternion& q) const
	{
		//	(Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
		//	(Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
		//	(Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
		//	(Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2)
		return Quaternion(
			w*q.x + x*q.w + y*q.z - z*q.y,
			w*q.y - x*q.z + y*q.w + z*q.x,
			w*q.z + x*q.y - y*q.x + z*q.w,
			w*q.w - x*q.x - y*q.y - z*q.z);
	}

	Quaternion Quaternion::Mul(const float scalar) const
	{
		return Quaternion(x*scalar, y*scalar, z*scalar, w*scalar);
	}

	float Quaternion::Length() const
	{
		return sqrt(LengthSquared());
	}

	float Quaternion::LengthSquared() const
	{
		return x*x + y*y + z*z + w*w;
	}

	float Quaternion::LengthManhattan() const
	{
		return abs(x) + abs(y) + abs(z) + abs(w);
	}

	Quaternion Quaternion::Normalize() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(1.0f / sqrt(len));

		return Quaternion(0, 0, 0, 0);
	}

	Quaternion Quaternion::NormalizeFast() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(invsqrt(len));

		return Quaternion(0, 0, 0, 0);
	}

	float Quaternion::Dot(const Quaternion& q) const
	{
		return x*q.x + y*q.y + z*q.z + w*q.w;
	}

	Quaternion Quaternion::Inverse() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Quaternion(-x, -y, -z, w).Mul(invsqrt(len));

		return Quaternion(0, 0, 0, 0);
	}

	Quaternion& Quaternion::AxisAngle(float _x, float _y, float _z, float _degrees)
	{
		float a = (float)DEGREES_TO_RADIANS * _degrees * 0.5f;
		float r = ::sin(a);

		x = _x * r;
		y = _y * r;
		z = _z * r;
		w = ::cos(a);

		return *this;
	}

	Quaternion& Quaternion::AxisAngle()
	{
		return AxisAngle(x, y, z, w);
	}

	Quaternion& Quaternion::Euler(float _pitch, float _yaw, float _roll)
	{
		float DR = (float)DEGREES_TO_RADIANS * 0.5f;

		float ex = DR * _pitch;
		float ey = DR * _yaw;
		float ez = DR * _roll;

		float cx = cos(ex);
		float cy = cos(ey);
		float cz = cos(ez);

		float sx = sin(ex);
		float sy = sin(ey);
		float sz = sin(ez);

		float cycz = cy * cz;
		float sysz = sy * sz;
		float cysz = cy * sz;
		float sycz = sy * cz;

		x = sx * cycz - cx * sysz;
		y = cx * sycz + sx * cysz;
		z = cx * cysz - sx * sycz;
		w = cx * cycz + sx * sysz;

		return *this;
	}

	Quaternion& Quaternion::Euler()
	{
		return Euler(x, y, z);
	}

	Vector3 Quaternion::ToEuler()
	{
		Vector3 v;

		float y_y = y*y;
		float py = w*y - z*x;

		v.x = (float)atan2(
			2 * (x*w + y*z),
			1 - 2 * (x*x + y_y));

		v.y = (float)asin(2 * py);

		v.z = (float)atan2(
			2 * (z*w + y*x),
			1 - 2 * (y_y + z*z));

		if (py >= 0.499f)
		{
			v.x = (float)(-2 * atan2(x, w));
			v.z = 0;
		}
		else if (py <= -0.499f)
		{
			v.x = (float)(2 * atan2(x, w));
			v.z = 0;
		}

		for (int i = 0; i < 3; ++i)
			if (v[i] > -EPSILON && v[i] < EPSILON)
				v[i] = 0;

		return v * (float)RADIANS_TO_DEGREES;
	}

	Vector3 Quaternion::Transform(const Vector3& v) const
	{
		Quaternion q = ((*this) * Quaternion(v.x, v.y, v.z, 0)) * Quaternion(-x, -y, -z, w);

		return Vector3(q.x, q.y, q.z);
	}

	Quaternion Quaternion::Slerp(const Quaternion& q, float t)
	{
		float product = Dot(q) * invsqrt(LengthSquared() * q.LengthSquared());

		if (fabs(product) != 1.0f)
		{
			float sign = (product < 0.0f) ? -1.0f : 1.0f;

			float theta = acos(sign * product);
			float s0 = sin((1.0f - t) * theta);
			float s1 = sin(sign * t * theta);
			float d = 1.0f / sin(theta);

			return Quaternion(
				(x * s0 + q.x * s1) * d,
				(y * s0 + q.y * s1) * d,
				(z * s0 + q.z * s1) * d,
				(w * s0 + q.w * s1) * d);
		}
		else
		{
			return *this;
		}
	}

	// Based on code by Nic @
	// http://nic-gamedev.blogspot.com/2011/11/quaternion-math-getting-local-axis.html
	//
	Vector3 Quaternion::Forward() const
	{
		return Vector3(
			-2 * (x * z + w * y),
			-2 * (y * z - w * x),
			-1 + 2 * (x * x + y * y)).NormalizeFast();
	}

	Vector3 Quaternion::Up() const
	{
		return Vector3(
			2 * (x * y - w * z),
			1 - 2 * (x * x + z * z),
			2 * (y * z + w * x)).NormalizeFast();
	}

	Vector3 Quaternion::Right() const
	{
		return Vector3(
			1 - 2 * (y * y + z * z),
			2 * (x * y + w * z),
			2 * (x * z - w * y)).NormalizeFast();
	}
}
