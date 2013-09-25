#pragma once

#include "Property.h"
#include "Shape.h"
#include "WVector3f.h"
#include "WBuffer.h"

using namespace Sentinel::Assets;

namespace Sentinel { namespace Math
{
	public ref class WRay
	{
		DECLARE_REF( Ray );

	protected:

		WRay();

	public:

		WRay( WVector3f^ pos, WVector3f^ dir );
		
		/////////////////////////////////

		DECLARE_PROPERTY( WVector3f^, Position );
		DECLARE_PROPERTY( WVector3f^, Direction );

		/////////////////////////////////

		DECLARE_OP_DEREF( Ray );
	};

	DECLARE_CLASS_REF( Ray );

	//////////////////////////////////////////////////////

	public ref class WPlane
	{
		DECLARE_REF( Plane );

	public:

		WPlane();
		WPlane( WVector3f^ pos, WVector3f^ normal );

		/////////////////////////////////

		DECLARE_PROPERTY( WVector3f^, Position );
		DECLARE_PROPERTY( WVector3f^, Normal );

		/////////////////////////////////

		DECLARE_OP_DEREF( Plane );

		float		Distance( WVector3f^ pos );
	};

	DECLARE_CLASS_REF( Plane );

	//////////////////////////////////////////////////////

	public ref class WTriangle
	{
		DECLARE_REF( Triangle );

	protected:

		WTriangle();

	public:

		WTriangle( WVector3f^ posA, WVector3f^ posB, WVector3f^ posC );

		/////////////////////////////////

		DECLARE_OP_DEREF( Triangle );

		RVector3f^	Position( int index );

		/////////////////////////////////

		float		Area();
	};

	DECLARE_CLASS_REF( Triangle );

	//////////////////////////////////////////////////////

	public ref class WBoundingSphere
	{
		DECLARE_REF( BoundingSphere );

	public:

		WBoundingSphere();
		WBoundingSphere( WVector3f^ center, float radius );
		WBoundingSphere( WVector3f^ v0, WVector3f^ v1 );
		WBoundingSphere( WVector3f^ v0, WVector3f^ v1, WVector3f^ v2 );
		WBoundingSphere( WVector3f^ v0, WVector3f^ v1, WVector3f^ v2, WVector3f^ v3 );
		WBoundingSphere( System::IntPtr^ data, UINT count, UINT stride );
		WBoundingSphere( const BoundingSphere& sphere );
		
		/////////////////////////////////

		DECLARE_PROPERTY( WVector3f^, Center );
		DECLARE_PROPERTY( float,	  Radius );

		/////////////////////////////////

		DECLARE_OP_DEREF( BoundingSphere );

		bool			Intersects( WVector3f^ point );
		bool			Intersects( WBoundingSphere^ sphere );
	};

	DECLARE_CLASS_REF( BoundingSphere );

	//////////////////////////////////////////////////////

	public ref class WBoundingBox
	{
		DECLARE_REF( BoundingBox );

	protected:

		WBoundingBox();

	public:

		WBoundingBox( WVector3f^ minBounds, WVector3f^ maxBounds );
		WBoundingBox( System::IntPtr^ data, UINT count, UINT stride );
		WBoundingBox( const BoundingBox& box );

		/////////////////////////////////

		DECLARE_OP_DEREF( BoundingBox );

		void			Set( WVector3f^ minBounds, WVector3f^ maxBounds );

		WVector3f^		GetMinBounds();
		WVector3f^		GetMaxBounds();

		/////////////////////////////////

		bool			Intersects( WVector3f^ point );
		bool			Intersects( WRay^ ray );
	};

	DECLARE_CLASS_REF( BoundingBox );
}}
