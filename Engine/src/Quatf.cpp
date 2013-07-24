#include "MathCommon.h"
#include "Quatf.h"

namespace Sentinel
{
	Quatf::Quatf()
	{
		x = y = z = 0;
		w = 1;
	}

	Quatf::Quatf( float _pitch, float _yaw, float _roll )
	{
		Euler( _pitch, _yaw, _roll );
	}

	Quatf::Quatf( float _x, float _y, float _z, float _w )
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	float Quatf::X() const
	{
		return x;
	}

	float Quatf::Y() const
	{
		return y;
	}

	float Quatf::Z() const
	{
		return z;
	}

	float Quatf::W() const
	{
		return w;
	}

	void Quatf::SetX( float _x )
	{
		x = _x;
	}

	void Quatf::SetY( float _y )
	{
		y = _y;
	}

	void Quatf::SetZ( float _z )
	{
		z = _z;
	}

	void Quatf::SetW( float _w )
	{
		w = _w;
	}

	float* Quatf::Ptr()
	{
		return static_cast< float* >(&x);
	}

	bool Quatf::operator == ( const Quatf& v ) const
	{
		return Equals( v );
	}

	bool Quatf::operator != ( const Quatf& v ) const
	{
		return !Equals( v );
	}

	bool Quatf::Equals( const Quatf& v ) const
	{
		return (x == v.x && y == v.y && z == v.z && w == v.w);
	}

	Quatf Quatf::operator * ( const Quatf& q ) const
	{
		return Mul( q );
	}

	Quatf Quatf::Mul( const Quatf& q ) const
	{
		//	(Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
		//	(Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
		//	(Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
		//	(Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2)
		return Quatf( w*q.x + x*q.w + y*q.z - z*q.y,
					  w*q.y - x*q.z + y*q.w + z*q.x,
					  w*q.z + x*q.y - y*q.x + z*q.w,
					  w*q.w - x*q.x - y*q.y - z*q.z );
	}

	Quatf Quatf::Mul( const float scalar ) const
	{
		return Quatf( x / scalar, y / scalar, z / scalar, w / scalar );
	}

	float Quatf::Length() const
	{
		return sqrt( LengthSquared() );
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

		if( len > 0 )
			return Mul( 1.0f / sqrt( len ));

		return Quatf( 0, 0, 0, 0 );
	}

	Quatf& Quatf::AxisAngle( float _x, float _y, float _z, float _degrees )
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
		return AxisAngle( x, y, z, w );
	}

	Quatf& Quatf::Euler( float _pitch, float _yaw, float _roll )
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
		return Euler( x, y, z );
	}
}