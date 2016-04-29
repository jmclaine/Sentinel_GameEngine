#pragma once

namespace Sentinel
{
	struct Point2
	{
		int x, y;

		Point2() :
			x(0), y(0)
		{ }

		Point2(int _x, int _y) :
			x(_x), y(_y)
		{ }

		int* Ptr()
		{
			return &x;
		}
	};

	struct Point3
	{
		int x, y, z;

		Point3() :
			x(0), y(0), z(0)
		{ }

		Point3(int _x, int _y, int _z) :
			x(_x), y(_y), z(_z)
		{ }

		int* Ptr()
		{
			return &x;
		}
	};

	struct Point4
	{
		int x, y, z, w;

		Point4() :
			x(0), y(0), z(0), w(0)
		{ }

		Point4(int _x, int _y, int _z, int _w) :
			x(_x), y(_y), z(_z), w(_w)
		{ }

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
			left(0), top(0), right(0), bottom(0)
		{ }

		Quad(float _left, float _top, float _right, float _bottom) :
			left(_left), top(_top), right(_right), bottom(_bottom)
		{ }

		float* Ptr()
		{
			return &left;
		}
	};
}
