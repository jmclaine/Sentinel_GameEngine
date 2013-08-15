#pragma once

#include "Common.h"
#include "Vector3f.h"
#include "Buffer.h"

namespace Sentinel
{
	class SENTINEL_DLL Plane
	{
	public:

		Vector3f mPosition,
				 mNormal;

		Plane( const Vector3f& pos = Vector3f( 0, 0, 0 ), const Vector3f& normal = Vector3f( 0, 0, 0 ));

		float Distance( const Vector3f& pos );
	};

	class SENTINEL_DLL Triangle
	{
	public:

		Vector3f mPosition[ 3 ];

		Triangle( const Vector3f& posA = Vector3f( 0, 0, 0 ), const Vector3f& posB = Vector3f( 0, 0, 0 ), const Vector3f& posC = Vector3f( 0, 0, 0 ));

		float Area();
	};

	class SENTINEL_DLL BoundingSphere
	{
	public:

		Vector3f mCenter;
		float    mRadius;

		BoundingSphere( const Vector3f& center = Vector3f( 0, 0, 0 ), float radius = 0 );
		BoundingSphere( const Vector3f& v0, const Vector3f& v1 );
		BoundingSphere( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2 );
		BoundingSphere( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 );
	};

	SENTINEL_DLL bool CheckPointInSphere( const Vector3f& point, const BoundingSphere& sphere );

	SENTINEL_DLL BoundingSphere FindSmallestSphere( Buffer*& vbo );
}
