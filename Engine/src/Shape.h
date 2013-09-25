#pragma once

#include "Common.h"
#include "Vector3f.h"

namespace Sentinel
{
	class SENTINEL_DLL Ray
	{
	public:

		Vector3f	mPosition;
		Vector3f	mDirection;

		Ray( const Vector3f& pos, const Vector3f& dir );
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL Plane
	{
	public:

		Vector3f	mPosition;
		Vector3f	mNormal;

		Plane( const Vector3f& pos = Vector3f(), const Vector3f& normal = Vector3f() );

		float		Distance( const Vector3f& pos );
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL Triangle
	{
	public:

		Vector3f	mPosition[ 3 ];

		Triangle( const Vector3f& posA, const Vector3f& posB, const Vector3f& posC );

		float		Area();
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL BoundingSphere
	{
	public:

		Vector3f	mCenter;
		float		mRadius;

		BoundingSphere( const Vector3f& center = Vector3f(), float radius = 0 );
		BoundingSphere( const Vector3f& v0, const Vector3f& v1 );
		BoundingSphere( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2 );
		BoundingSphere( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 );
		BoundingSphere( char* verts, UINT count, UINT stride = sizeof( Vector3f ));	// Takes in a pointer to 3 float points.

		bool		Intersects( const Vector3f& point );
		bool		Intersects( const BoundingSphere& sphere );
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL BoundingBox
	{
	private:

		Plane		mPlane[ 6 ];

	public:

		BoundingBox( const Vector3f& minBounds, const Vector3f& maxBounds );
		BoundingBox( char* verts, UINT count, UINT stride = sizeof( Vector3f ));	// Takes in a pointer to 3 float points.

		void		Set( const Vector3f& minBounds, const Vector3f& maxBounds );

		const Vector3f& GetMinBounds() const;
		const Vector3f& GetMaxBounds() const;

		bool		Intersects( const Vector3f& point );
		bool		Intersects( const Ray& ray );
	};
}
