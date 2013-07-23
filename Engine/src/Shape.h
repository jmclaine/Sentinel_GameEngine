#pragma once

#include "Common.h"
#include "Vector3f.h"
#include "Buffer.h"

namespace Sentinel
{
	class SENTINEL_DLL Plane
	{
	public:

		Vector3f pos,
				 normal;

		Plane( const Vector3f& _pos = Vector3f( 0, 0, 0 ), const Vector3f& _normal = Vector3f( 0, 0, 0 ));

		float Distance( const Vector3f& _pos );
	};

	class SENTINEL_DLL Triangle
	{
	public:

		Vector3f pos[ 3 ];

		Triangle( const Vector3f& _posA = Vector3f( 0, 0, 0 ), const Vector3f& _posB = Vector3f( 0, 0, 0 ), const Vector3f& _posC = Vector3f( 0, 0, 0 ));

		float Area();
	};

	class SENTINEL_DLL BoundingSphere
	{
	public:

		Vector3f center;
		float    radius;

		BoundingSphere( const Vector3f& _center = Vector3f( 0, 0, 0 ), float _radius = 0 );
		BoundingSphere( const Vector3f& v0, const Vector3f& v1 );
		BoundingSphere( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2 );
		BoundingSphere( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 );
	};

	SENTINEL_DLL bool CheckPointInSphere( const Vector3f& v, const BoundingSphere& sphere );

	SENTINEL_DLL BoundingSphere FindSmallestSphere( Buffer*& vbo );
}
