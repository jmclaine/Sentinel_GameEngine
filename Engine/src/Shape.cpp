#include <vector>

#include "MathCommon.h"
#include "Shape.h"

namespace Sentinel
{
	Ray::Ray( const Vector3f& pos, const Vector3f& dir ) :
		mPosition( pos ),
		mDirection( dir )
	{}

	/////////////////////////////////////////////////////////////////////////////////////

	Plane::Plane( const Vector3f& pos, const Vector3f& normal ) :
		mPosition( pos ),
		mNormal( normal )
	{}

	float Plane::Distance( const Vector3f& pos )
	{
		return mNormal.Dot( pos ) - mPosition.Dot( mNormal );
	}

	/////////////////////////////////////////////////////////////////////////////////////

	Triangle::Triangle( const Vector3f& posA, const Vector3f& posB, const Vector3f& posC )
	{
		mPosition[ 0 ] = posA;
		mPosition[ 1 ] = posB;
		mPosition[ 2 ] = posC;
	}

	// Based on Heron's Formula @ www.mathopenref.com/heronsformula.html
	//
	float Triangle::Area()
	{
		float a = (mPosition[ 0 ] - mPosition[ 1 ]).Length();
		float b = (mPosition[ 0 ] - mPosition[ 2 ]).Length();
		float c = (mPosition[ 1 ] - mPosition[ 2 ]).Length();
		float p = (a + b + c) / 2;

		return sqrt( p * (p - a) * (p - b) * (p - c) );
	}

	/////////////////////////////////////////////////////////////////////////////////////
	
	BoundingSphere::BoundingSphere( const Vector3f& center, float radius ) :
		mCenter( center ),
		mRadius( radius )
	{}

	// Create a sphere based on points contained within it.
	// Based on code by:
	// Nicolas Capens @ www.flipcode.com/archives/Smallest_Enclosing_Spheres.html
	//
	BoundingSphere::BoundingSphere( const Vector3f& v0, const Vector3f& v1 )
	{
		Vector3f u1 = v1 - v0;
		Vector3f u0 = u1 * 0.5f;

		mRadius = u0.Length() + (float)EPSILON;
		mCenter = v0 + u0;
	}

	BoundingSphere::BoundingSphere( const Vector3f& v0, const Vector3f& v1,  const Vector3f& v2 )
	{
		Vector3f u1 = v1 - v0;
		Vector3f u2 = v2 - v0;

		float det = 2.0f * u1.Cross( u2 ).Dot( u1.Cross( u2 ));

		// Guarantee a smallest sphere of two points.
		//
		if( det == 0 )
		{
			BoundingSphere sphere;

			float len01 = u1.LengthSquared();
			float len02 = u2.LengthSquared();

			if( len01 > len02 )
			{
				sphere = BoundingSphere( v0, v1 );
			}
			else
			{
				sphere = BoundingSphere( v0, v2 );
			}

			mRadius = sphere.mRadius;
			mCenter = sphere.mCenter;

			return;
		}

		Vector3f u0 = (u1.Cross( u2 ).Cross( u1 ) * u2.LengthSquared() + \
					   u2.Cross( u1.Cross( u2 )) * u1.LengthSquared()) * (1.0f / det);

		mRadius = u0.Length() + (float)EPSILON;
		mCenter = v0.Add( u0 );
	}

	BoundingSphere::BoundingSphere( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 )
	{
		Vector3f u1 = v1 - v0;
		Vector3f u2 = v2 - v0;
		Vector3f u3 = v3 - v0;

		float m[9];
		m[0] = u1.x;	m[1] = u1.y;	m[2] = u1.z;
		m[3] = u2.x;	m[4] = u2.y;	m[5] = u2.z;
		m[6] = u3.x;	m[7] = u3.y;	m[8] = u3.z;
		float det = 2.0f * m[0]*(m[4]*m[8]-m[5]*m[7]) - m[1]*(m[3]*m[8]-m[5]*m[6]) + m[2]*(m[3]*m[7]-m[4]*m[6]);

		// Guarantee a smallest sphere of three points.
		//
		if( det == 0 )
		{
			BoundingSphere sphere;
			float tri012 = Triangle( v0, v1, v2 ).Area();
			float tri013 = Triangle( v0, v1, v3 ).Area();
			float tri023 = Triangle( v0, v2, v3 ).Area();

			if( tri012 > tri013 && tri012 > tri023 )
			{
				sphere = BoundingSphere( v0, v1, v2 );
			}
			else
			if( tri013 > tri012 && tri013 > tri023 )
			{
				sphere = BoundingSphere( v0, v1, v3 );
			}
			else
			{
				sphere = BoundingSphere( v0, v2, v3 );
			}

			mRadius = sphere.mRadius;
			mCenter = sphere.mCenter;
			return;
		}

		Vector3f u0 = (u1.Cross( u2 ) * u3.LengthSquared() + \
					   u3.Cross( u1 ) * u2.LengthSquared() + \
					   u2.Cross( u3 ) * u1.LengthSquared()) * (1.0f / det);

		mRadius = u0.Length() + (float)EPSILON;
		mCenter = v0 + u0;
	}

	// Find the smallest sphere.
	// Based on code by:
	// Nicolas Capens @ www.flipcode.com/archives/Smallest_Enclosing_Spheres.html
	// Guest @ pastebin.com/04b1GBA2
	//
	BoundingSphere::BoundingSphere( char* verts, UINT count, UINT stride )
	{
		std::vector< Vector3f > points;		// points of interest
		mCenter = *(Vector3f*)verts;
		
		points.push_back( mCenter );

		Vector3f pos;
		UINT currIndex = 0;

		while( currIndex < count )
		{
			pos = *(Vector3f*)(verts + currIndex * stride);

			if( !Intersects( pos ))
			{
				BoundingSphere minSphere;
				BoundingSphere newSphere;
				int index = -1;

				switch( points.size() )
				{
					case 1:
						minSphere = BoundingSphere( pos, points[ 0 ] );
						points.push_back( pos );
						break;

					case 2:
						newSphere = BoundingSphere( pos, points[ 0 ] );
						if( newSphere.Intersects( points[ 1 ] ))
						{
							minSphere = newSphere;
							index = 0;
						}

						newSphere = BoundingSphere( pos, points[ 1 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ))
						{
							minSphere = newSphere;
							index = 1;
						}

						if( index > -1 )
						{
							points[ 1 - index ] = pos;
						}
						else
						{
							minSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ] );
							points.push_back( pos );
						}
						break;

					case 3:
						newSphere = BoundingSphere( pos, points[ 0 ] );
						if( newSphere.Intersects( points[ 1 ] ) &&
							newSphere.Intersects( points[ 2 ] ))
						{
							minSphere = newSphere;
							index = 0;
						}

						newSphere = BoundingSphere( pos, points[ 1 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ) &&
							newSphere.Intersects( points[ 2 ] ))
						{
							minSphere = newSphere;
							index = 1;
						}

						newSphere = BoundingSphere( pos, points[ 2 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ) &&
							newSphere.Intersects( points[ 1 ] ))
						{
							minSphere = newSphere;
							index = 2;
						}

						newSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 2 ] ))
						{
							minSphere = newSphere;
							index = 3;
						}

						newSphere = BoundingSphere( pos, points[ 0 ], points[ 2 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 1 ] ))
						{
							minSphere = newSphere;
							index = 4;
						}

						newSphere = BoundingSphere( pos, points[ 1 ], points[ 2 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ))
						{
							minSphere = newSphere;
							index = 5;
						}

						switch( index )
						{
							case 0:
								points.resize( 2 );
								points[ 1 ] = pos;
								break;

							case 1:
								points.resize( 2 );
								points[ 0 ] = pos;
								break;

							case 2:
								points[ 0 ] = points[ 2 ];
								points.resize( 2 );
								points[ 1 ] = pos;
								break;

							case 3:
								points[ 2 ] = pos;
								break;

							case 4:
								points[ 1 ] = pos;
								break;

							case 5:
								points[ 0 ] = pos;
								break;

							default:
								minSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ], points[ 2 ] );
								points.push_back( pos );
								break;
						}
						break;

					case 4:
						// Check 2 point spheres.
						//
						newSphere = BoundingSphere( pos, points[ 0 ] );
						if( newSphere.Intersects( points[ 1 ] ) &&
							newSphere.Intersects( points[ 2 ] ) &&
							newSphere.Intersects( points[ 3 ] ))
						{
							minSphere = newSphere;
							index = 0;
						}

						newSphere = BoundingSphere( pos, points[ 1 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ) &&
							newSphere.Intersects( points[ 2 ] ) &&
							newSphere.Intersects( points[ 3 ] ))
						{
							minSphere = newSphere;
							index = 1;
						}

						newSphere = BoundingSphere( pos, points[ 2 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ) &&
							newSphere.Intersects( points[ 1 ] ) &&
							newSphere.Intersects( points[ 3 ] ))
						{
							minSphere = newSphere;
							index = 2;
						}

						newSphere = BoundingSphere( pos, points[ 3 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ) &&
							newSphere.Intersects( points[ 1 ] ) &&
							newSphere.Intersects( points[ 2 ] ))
						{
							minSphere = newSphere;
							index = 3;
						}

						// Check 3 point spheres.
						//
						newSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 2 ] ) &&
							newSphere.Intersects( points[ 3 ] ))
						{
							minSphere = newSphere;
							index = 4;
						}

						newSphere = BoundingSphere( pos, points[ 0 ], points[ 2 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 1 ] ) &&
							newSphere.Intersects( points[ 3 ] ))
						{
							minSphere = newSphere;
							index = 5;
						}

						newSphere = BoundingSphere( pos, points[ 0 ], points[ 3 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 1 ] ) &&
							newSphere.Intersects( points[ 2 ] ))
						{
							minSphere = newSphere;
							index = 6;
						}

						newSphere = BoundingSphere( pos, points[ 1 ], points[ 2 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ) &&
							newSphere.Intersects( points[ 3 ] ))
						{
							minSphere = newSphere;
							index = 7;
						}

						newSphere = BoundingSphere( pos, points[ 1 ], points[ 3 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ) &&
							newSphere.Intersects( points[ 2 ] ))
						{
							minSphere = newSphere;
							index = 8;
						}

						newSphere = BoundingSphere( pos, points[ 2 ], points[ 3 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ) &&
							newSphere.Intersects( points[ 1 ] ))
						{
							minSphere = newSphere;
							index = 9;
						}

						// Check 4 point spheres.
						//
						newSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ], points[ 2 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 3 ] ))
						{
							minSphere = newSphere;
							index = 10;
						}

						newSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ], points[ 3 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 2 ] ))
						{
							minSphere = newSphere;
							index = 11;
						}

						newSphere = BoundingSphere( pos, points[ 1 ], points[ 2 ], points[ 3 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 0 ] ))
						{
							minSphere = newSphere;
							index = 12;
						}

						newSphere = BoundingSphere( pos, points[ 0 ], points[ 2 ], points[ 3 ] );
						if( newSphere.mRadius < minSphere.mRadius &&
							newSphere.Intersects( points[ 1 ] ))
						{
							minSphere = newSphere;
							index = 13;
						}

						switch( index )
						{
							case 0:
								points.resize( 2 );
								points[ 1 ] = pos;
								break;

							case 1:
								points.resize( 2 );
								points[ 0 ] = pos;
								break;

							case 2:
								points[ 0 ] = points[ 2 ];
								points.resize( 2 );
								points[ 1 ] = pos;
								break;

							case 3:
								points[ 0 ] = points[ 3 ];
								points.resize( 2 );
								points[ 1 ] = pos;
								break;

							case 4:
								points.resize( 3 );
								points[ 2 ] = pos;
								break;

							case 5:
								points.resize( 3 );
								points[ 1 ] = pos;
								break;

							case 6:
								points[ 1 ] = points[ 3 ];
								points.resize( 3 );
								points[ 2 ] = pos;
								break;

							case 7:
								points[ 0 ] = points[ 2 ];
								points.resize( 3 );
								points[ 2 ] = pos;
								break;

							case 8:
								points[ 0 ] = points[ 3 ];
								points.resize( 3 );
								points[ 2 ] = pos;
								break;

							case 9:
								points[ 0 ] = points[ 3 ];
								points[ 1 ] = pos;
								points.resize( 3 );
								break;

							case 10:
								points[ 3 ] = pos;
								break;

							case 11:
								points[ 2 ] = pos;
								break;

							case 12:
								points[ 0 ] = pos;
								break;

							case 13:
								points[ 1 ] = pos;
								break;
						}
						break;
				}

				if( minSphere.mRadius > mRadius )
				{
					mCenter = minSphere.mCenter;
					mRadius = minSphere.mRadius;

					// Must recheck all points to be certain they
					// all fit within the newly created sphere.
					//
					currIndex = 0;
					continue;
				}
			}
		
			++currIndex;
		}
	}

	bool BoundingSphere::Intersects( const Vector3f& point )
	{
		return (point - mCenter).LengthSquared() < (mRadius * mRadius);
	}

	bool BoundingSphere::Intersects( const BoundingSphere& sphere )
	{
		float d = (mCenter - sphere.mCenter).LengthSquared();
		float r = (mRadius + sphere.mRadius);

		return (d < r*r);
	}

	/////////////////////////////////////////////////////////////////////////////////////

	BoundingBox::BoundingBox( const Vector3f& minBounds, const Vector3f& maxBounds )
	{
		Set( minBounds, maxBounds );
	}

	BoundingBox::BoundingBox( char* verts, UINT count, UINT stride )
	{
		Vector3f minBounds( FLT_MAX, FLT_MAX, FLT_MAX );
		Vector3f maxBounds( FLT_MIN, FLT_MIN, FLT_MIN );
		
		for( UINT x = 0; x < count; ++x )
		{
			Vector3f pos = *(Vector3f*)(verts + count * stride);

			for( int y = 0; y < 3; ++y )
			{
				if( pos[ y ] < minBounds[ y ] )
					minBounds[ y ] = pos[ y ];

				if( pos[ y ] > maxBounds[ y ] )
					maxBounds[ y ] = pos[ y ];
			}
		}

		Set( minBounds, maxBounds );
	}

	void BoundingBox::Set( const Vector3f& minBounds, const Vector3f& maxBounds )
	{
		// min
		mPlane[ 0 ].mPosition = minBounds;
		mPlane[ 0 ].mNormal   = Vector3f( 1, 0, 0 );

		mPlane[ 1 ].mPosition = minBounds;
		mPlane[ 1 ].mNormal   = Vector3f( 0, 1, 0 );

		mPlane[ 2 ].mPosition = minBounds;
		mPlane[ 2 ].mNormal   = Vector3f( 0, 0, 1 );

		// max
		mPlane[ 3 ].mPosition = maxBounds;
		mPlane[ 3 ].mNormal   = Vector3f( -1, 0, 0 );

		mPlane[ 4 ].mPosition = maxBounds;
		mPlane[ 4 ].mNormal   = Vector3f( 0, -1, 0 );

		mPlane[ 5 ].mPosition = maxBounds;
		mPlane[ 5 ].mNormal   = Vector3f( 0, 0, -1 );
	}

	const Vector3f& BoundingBox::GetMinBounds() const
	{
		return mPlane[ 0 ].mPosition;
	}

	const Vector3f& BoundingBox::GetMaxBounds() const
	{
		return mPlane[ 3 ].mPosition;
	}

	bool BoundingBox::Intersects( const Vector3f& point )
	{
		for( UINT x = 0; x < 3; ++x )
			if( point[ x ] < GetMinBounds()[ x ] || point[ x ] > GetMaxBounds()[ x ] )
				return false;

		return true;
	}

	// Check for a Ray being within a Box.
	//
	// Based on Dan Sunday's algorithm.
	// softsurfer.com/Archive/algorithm_0105/algorithm_0105.htm
	//
	bool BoundingBox::Intersects( const Ray& ray )
	{
		for( int axis = 0; axis < 6; ++axis )
		{
			Vector3f w0 = ray.mPosition - mPlane[ axis ].mPosition;
			float a = -mPlane[ axis ].mNormal.Dot( w0 );
			float b = mPlane[ axis ].mNormal.Dot( ray.mDirection );

			//if (fabs(b) < 0.001f)	// ray is parallel to the plane
			//	return false;

			float r = a / b;
			//if (r < 0.0f)			// ray goes away from plane
			//	return false;

			// add a little to ensure the point has a chance to be within the box
			Vector3f point = ray.mPosition + ray.mDirection * r + mPlane[ axis ].mNormal * 0.001f;

			if( Intersects( point ))
				return true;
		}

		return false;
	}
}
