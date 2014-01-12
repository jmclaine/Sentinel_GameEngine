#pragma once

namespace Sentinel
{
	struct Point2i
	{
		int x, y;

		Point2i() :
			x( 0 ), y( 0 )
		{}

		Point2i( int _x, int _y ) :
			x( _x ), y( _y )
		{}

		int* Ptr()
		{
			return &x;
		}
	};

	struct Point3i
	{
		int x, y, z;

		Point3i() :
			x( 0 ), y( 0 ), z( 0 )
		{}

		Point3i( int _x, int _y, int _z ) :
			x( _x ), y( _y ), z( _z )
		{}

		int* Ptr()
		{
			return &x;
		}
	};

	struct Point4i
	{
		int x, y, z, w;

		Point4i() :
			x( 0 ), y( 0 ), z( 0 ), w( 0 )
		{}

		Point4i( int _x, int _y, int _z, int _w ) :
			x( _x ), y( _y ), z( _z ), w( _w )
		{}

		int* Ptr()
		{
			return &x;
		}
	};

	struct Quad
	{
		float left, top;
		float right, bottom;

		Quad() :
			left( 0 ), top( 0 ), right( 0 ), bottom( 0 )
		{}

		Quad( float _left, float _top, float _right, float _bottom ) :
			left( _left ), top( _top ), right( _right ), bottom( _bottom )
		{}

		float* Ptr()
		{
			return &left;
		}
	};
}
