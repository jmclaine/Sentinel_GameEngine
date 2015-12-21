#include "MathUtil.h"
#include "Quatf.h"
#include "StringStream.h"

namespace Sentinel
{
	const Quatf Quatf::IDENTITY = Quatf(0, 0, 0, 1);

	////////////////////////////////

	Quatf::Quatf() : 
		x(0), y(0), z(0), w(1)
	{}

	Quatf::Quatf(float _pitch, float _yaw, float _roll)
	{
		Euler(_pitch, _yaw, _roll);
	}

	Quatf::Quatf(const Vector3f& rotation)
	{
		Euler(rotation.x, rotation.y, rotation.z);
	}

	Quatf::Quatf(float _x, float _y, float _z, float _w) :
		x(_x), y(_y), z(_z), w(_w)
	{}

	float* Quatf::Ptr() const
	{
		return const_cast<float*>(&x);
	}

	std::string Quatf::ToString() const
	{
		return STREAM("(" << x << ", " << y << ", " << z << ", " << w << ")");
	}

	Quatf Quatf::operator - () const
	{
		return Inverse();
	}

	bool Quatf::operator == (const Quatf& v) const
	{
		return Equals(v);
	}

	bool Quatf::operator != (const Quatf& v) const
	{
		return !Equals(v);
	}

	bool Quatf::Equals(const Quatf& v) const
	{
		return (x == v.x && y == v.y && z == v.z && w == v.w);
	}

	Quatf& Quatf::operator *= (const Quatf& q)
	{
		*this = Mul(q);
		return *this;
	}

	Quatf Quatf::operator * (const Quatf& q) const
	{
		return Mul(q);
	}

	Quatf Quatf::Mul(const Quatf& q) const
	{
		//	(Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
		//	(Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
		//	(Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
		//	(Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2)
		return Quatf(
			w*q.x + x*q.w + y*q.z - z*q.y,
			w*q.y - x*q.z + y*q.w + z*q.x,
			w*q.z + x*q.y - y*q.x + z*q.w,
			w*q.w - x*q.x - y*q.y - z*q.z);
	}

	Quatf Quatf::Mul(const float scalar) const
	{
		return Quatf(x*scalar, y*scalar, z*scalar, w*scalar);
	}

	float Quatf::Length() const
	{
		return sqrt(LengthSquared());
	}

	float Quatf::LengthSquared() const
	{
		return x*x + y*y + z*z + w*w;
	}

	float Quatf::LengthManhattan() const
	{
		return abs(x) + abs(y) + abs(z) + abs(w);
	}

	Quatf Quatf::Normalize() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(1.0f / sqrt(len));

		return Quatf(0, 0, 0, 0);
	}

	Quatf Quatf::NormalizeFast() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Mul(invsqrt(len));

		return Quatf(0, 0, 0, 0);
	}

	float Quatf::Dot(const Quatf& q) const
	{
		return x*q.x + y*q.y + z*q.z + w*q.w;
	}

	Quatf Quatf::Inverse() const
	{
		float len = LengthSquared();

		if (len > 0)
			return Quatf(-x, -y, -z, w).Mul(invsqrt(len));

		return Quatf(0, 0, 0, 0);
	}

	Quatf& Quatf::AxisAngle(float _x, float _y, float _z, float _degrees)
	{
		float a = (float)DEGREES_TO_RADIANS * _degrees * 0.5f;
		float r = ::sin(a);

		x = _x * r;
		y = _y * r;
		z = _z * r;
		w = ::cos(a);

		return *this;
	}

	Quatf& Quatf::AxisAngle()
	{
		return AxisAngle(x, y, z, w);
	}

	Quatf& Quatf::Euler(float _pitch, float _yaw, float _roll)
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

	Quatf& Quatf::Euler()
	{
		return Euler(x, y, z);
	}

	Vector3f Quatf::ToEuler()
	{
		Vector3f v;

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

	Vector3f Quatf::Transform(const Vector3f& v) const
	{
		Quatf q = ((*this) * Quatf(v.x, v.y, v.z, 0)) * Quatf(-x, -y, -z, w);

		return Vector3f(q.x, q.y, q.z);
	}

	Quatf Quatf::Slerp(const Quatf& q, float t)
	{
		float product = Dot(q) * invsqrt(LengthSquared() * q.LengthSquared());

		if (fabs(product) != 1.0f)
		{
			float sign = (product < 0.0f) ? -1.0f : 1.0f;

			float theta = acos(sign * product);
			float s0 = sin((1.0f - t) * theta);
			float s1 = sin(sign * t * theta);
			float d = 1.0f / sin(theta);

			return Quatf(
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
	Vector3f Quatf::Forward() const
	{
		return Vector3f(
			-2 * (x * z + w * y),
			-2 * (y * z - w * x),
			-1 + 2 * (x * x + y * y)).NormalizeFast();
	}

	Vector3f Quatf::Up() const
	{
		return Vector3f(
			2 * (x * y - w * z),
			1 - 2 * (x * x + z * z),
			2 * (y * z + w * x)).NormalizeFast();
	}

	Vector3f Quatf::Right() const
	{
		return Vector3f(
			1 - 2 * (y * y + z * z),
			2 * (x * y + w * z),
			2 * (x * z - w * y)).NormalizeFast();
	}
}
