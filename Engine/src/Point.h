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
	};
}
