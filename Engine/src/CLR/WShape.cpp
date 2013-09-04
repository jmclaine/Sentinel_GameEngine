#include "WShape.h"

namespace Sentinel { namespace Math
{
	DEFINE_REF( Ray );

	WRay::WRay()
	{
		mRef = NULL;
	}

	WRay::WRay( WVector3f^ pos, WVector3f^ dir )
	{
		mRef = new Ray( pos, dir );
	}

	DEFINE_PROPERTY_R( Ray, Vector3f, Position );
	DEFINE_PROPERTY_R( Ray, Vector3f, Direction );

	DEFINE_OP_DEREF( Ray );

	DEFINE_CLASS_REF( Ray );

	//////////////////////////////////////////////////////

	DEFINE_REF( Plane );

	WPlane::WPlane()
	{
		mRef = new Plane();
	}

	WPlane::WPlane( WVector3f^ pos, WVector3f^ normal )
	{
		mRef = new Plane( pos, normal );
	}

	DEFINE_PROPERTY_R( Plane, Vector3f, Position );
	DEFINE_PROPERTY_R( Plane, Vector3f, Normal );

	DEFINE_OP_DEREF( Plane );

	float WPlane::Distance( WVector3f^ pos )
	{
		return mRef->Distance( pos );
	}

	DEFINE_CLASS_REF( Plane );

	//////////////////////////////////////////////////////

	DEFINE_REF( Triangle );

	WTriangle::WTriangle()
	{
		mRef = NULL;
	}

	WTriangle::WTriangle( WVector3f^ posA, WVector3f^ posB, WVector3f^ posC )
	{
		mRef = new Triangle( posA, posB, posC );
	}

	RVector3f^ WTriangle::Position( int index )
	{
		return gcnew RVector3f( &mRef->mPosition[ index ] );
	}

	DEFINE_OP_DEREF( Triangle );

	float WTriangle::Area()
	{
		return mRef->Area();
	}

	DEFINE_CLASS_REF( Triangle );

	//////////////////////////////////////////////////////

	DEFINE_REF( BoundingSphere );

	WBoundingSphere::WBoundingSphere()
	{
		mRef = new BoundingSphere();
	}

	WBoundingSphere::WBoundingSphere( WVector3f^ center, float radius )
	{
		mRef = new BoundingSphere( center, radius );
	}

	WBoundingSphere::WBoundingSphere( WVector3f^ v0, WVector3f^ v1 )
	{
		mRef = new BoundingSphere( v0, v1 );
	}

	WBoundingSphere::WBoundingSphere( WVector3f^ v0, WVector3f^ v1, WVector3f^ v2 )
	{
		mRef = new BoundingSphere( v0, v1, v2 );
	}

	WBoundingSphere::WBoundingSphere( WVector3f^ v0, WVector3f^ v1, WVector3f^ v2, WVector3f^ v3 )
	{
		mRef = new BoundingSphere( v0, v1, v2, v3 );
	}

	WBoundingSphere::WBoundingSphere( const BoundingSphere& sphere )
	{
		mRef = new BoundingSphere( sphere.mCenter, sphere.mRadius );
	}

	DEFINE_PROPERTY_R( BoundingSphere, Vector3f, Center );
	DEFINE_PROPERTY_M( BoundingSphere, float,	 Radius );

	DEFINE_OP_DEREF( BoundingSphere );

	bool WBoundingSphere::Intersects( WVector3f^ point )
	{
		return mRef->Intersects( point );
	}

	bool WBoundingSphere::Intersects( WBoundingSphere^ sphere )
	{
		return mRef->Intersects( sphere );
	}

	WBoundingSphere^ FindSmallestSphere( WBuffer^ vbo )
	{
		return gcnew WBoundingSphere( Sentinel::FindSmallestSphere( vbo ));
	}

	DEFINE_CLASS_REF( BoundingSphere );

	//////////////////////////////////////////////////////

	DEFINE_REF( BoundingBox );

	WBoundingBox::WBoundingBox()
	{
		mRef = NULL;
	}

	WBoundingBox::WBoundingBox( WVector3f^ minBounds, WVector3f^ maxBounds )
	{
		mRef = new BoundingBox( minBounds, maxBounds );
	}

	DEFINE_OP_DEREF( BoundingBox );

	void WBoundingBox::Set( WVector3f^ minBounds, WVector3f^ maxBounds )
	{
		mRef->Set( minBounds, maxBounds );
	}

	WVector3f^ WBoundingBox::GetMinBounds()
	{
		return gcnew WVector3f( mRef->GetMinBounds() );
	}

	WVector3f^ WBoundingBox::GetMaxBounds()
	{
		return gcnew WVector3f( mRef->GetMaxBounds() );
	}

	bool WBoundingBox::Intersects( WVector3f^ point )
	{
		return mRef->Intersects( point );
	}

	bool WBoundingBox::Intersects( WRay^ ray )
	{
		return mRef->Intersects( ray );
	}

	DEFINE_CLASS_REF( BoundingBox );
}}
