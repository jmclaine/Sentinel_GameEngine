#include <vector>

#include "MathCommon.h"
#include "Shape.h"

namespace Sentinel
{
	Plane::Plane( const Vector3f& _pos, const Vector3f& _normal ) :
		pos( _pos ),
		normal( _normal )
	{}

	float Plane::Distance( const Vector3f& _pos )
	{
		return normal.Dot( _pos ) - pos.Dot( normal );
	}

	/////////////////////////////////////////////////////////////////////////////////////

	Triangle::Triangle( const Vector3f& _posA, const Vector3f& _posB, const Vector3f& _posC )
	{
		pos[ 0 ] = _posA;
		pos[ 1 ] = _posB;
		pos[ 2 ] = _posC;
	}

	// Based on Heron's Formula @ www.mathopenref.com/heronsformula.html
	//
	float Triangle::Area()
	{
		float a = (pos[ 0 ] - pos[ 1 ]).Length();
		float b = (pos[ 0 ] - pos[ 2 ]).Length();
		float c = (pos[ 1 ] - pos[ 2 ]).Length();
		float p = (a + b + c) / 2;

		return sqrt( p * (p - a) * (p - b) * (p - c) );
	}

	/////////////////////////////////////////////////////////////////////////////////////

	BoundingSphere::BoundingSphere( const Vector3f& _center, float _radius ) :
		center( _center ),
		radius( _radius )
	{}

	// Create a sphere based on points contained within it.
	// Based on code by:
	// Nicolas Capens @ www.flipcode.com/archives/Smallest_Enclosing_Spheres.html
	//
	BoundingSphere::BoundingSphere( const Vector3f& v0, const Vector3f& v1 )
	{
		Vector3f u1 = v1 - v0;
		Vector3f u0 = u1 * 0.5f;

		radius = u0.Length() + (float)EPSILON;
		center = v0 + u0;
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

			radius = sphere.radius;
			center = sphere.center;
			return;
		}

		Vector3f u0 = (u1.Cross( u2 ).Cross( u1 ) * u2.LengthSquared() + \
					   u2.Cross( u1.Cross( u2 )) * u1.LengthSquared()) * (1.0f / det);

		radius = u0.Length() + (float)EPSILON;
		center = v0.Add( u0 );
	}

	BoundingSphere::BoundingSphere( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 )
	{
		Vector3f u1 = v1 - v0;
		Vector3f u2 = v2 - v0;
		Vector3f u3 = v3 - v0;

		float m[9];
		m[0] = u1.X();	m[1] = u1.Y();	m[2] = u1.Z();
		m[3] = u2.X();	m[4] = u2.Y();	m[5] = u2.Z();
		m[6] = u3.X();	m[7] = u3.Y();	m[8] = u3.Z();
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

			radius = sphere.radius;
			center = sphere.center;
			return;
		}

		Vector3f u0 = (u1.Cross( u2 ) * u3.LengthSquared() + \
					   u3.Cross( u1 ) * u2.LengthSquared() + \
					   u2.Cross( u3 ) * u1.LengthSquared()) * (1.0f / det);

		radius = u0.Length() + (float)EPSILON;
		center = v0 + u0;
	}

	bool CheckPointInSphere( const Vector3f& position, const BoundingSphere& sphere )
	{
		float d  = (position - sphere.center).LengthSquared();
		float r2 = sphere.radius * sphere.radius;

		return ( d < r2 ) ? true : false;
	}

	// Find the smallest sphere.
	// Based on code by:
	// Nicolas Capens @ www.flipcode.com/archives/Smallest_Enclosing_Spheres.html
	// Guest @ pastebin.com/04b1GBA2
	//
	BoundingSphere FindSmallestSphere( Buffer*& vbo )
	{
		unsigned char* verts = (unsigned char*)vbo->Lock();

		std::vector< Vector3f > points;					// points of interest
		BoundingSphere sphere( *(Vector3f*)verts );		// final bounding sphere
		BoundingSphere newSphere[ 14 ];					// create possible spheres

		points.push_back( sphere.center );

		UINT currIndex = 0;
		while( currIndex < vbo->mCount )
		{
			Vector3f pos = *(Vector3f*)(verts + currIndex * vbo->mStride);
			bool isContained = false;

			for( UINT x = 0; x < points.size(); ++x )
			{
				if( points[ x ].Equals( pos ))
					isContained = true;
			}

			if( !isContained )
			{
				if( !CheckPointInSphere( pos, sphere ))
				{
					BoundingSphere minSphere;
					float minRadius = FLT_MAX;
					int index = -1;

					switch( points.size() )
					{
						case 1:
							minSphere = BoundingSphere( pos, points[ 0 ] );
							points.push_back( pos );
							break;

						case 2:
							newSphere[ 0 ] = BoundingSphere( pos, points[ 0 ] );
							newSphere[ 1 ] = BoundingSphere( pos, points[ 1 ] );

							if( CheckPointInSphere( points[ 1 ], newSphere[ 0 ] ))
							{
								minRadius = newSphere[ 0 ].radius;
								index = 0;
							}
							else
							if( newSphere[ 1 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 1 ] ))
							{
								minRadius = newSphere[ 1 ].radius;
								index = 1;
							}

							if( index > -1 )
							{
								minSphere = newSphere[ index ];
								points[ 1 - index ] = pos;
							}
							else
							{
								minSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ] );
								points.push_back( pos );
							}
							break;

						case 3:
							for( UINT y = 0; y < points.size(); ++y )
							{
								newSphere[ y ] = BoundingSphere( pos, points[ y ] );
							}
							newSphere[ 3 ] = BoundingSphere( pos, points[ 0 ], points[ 1 ] );
							newSphere[ 4 ] = BoundingSphere( pos, points[ 0 ], points[ 2 ] );
							newSphere[ 5 ] = BoundingSphere( pos, points[ 1 ], points[ 2 ] );

							if( CheckPointInSphere( points[ 1 ], newSphere[ 0 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 0 ] ))
							{
								index = 0;
								minRadius = newSphere[ 0 ].radius;
							}

							if( newSphere[ 1 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 1 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 1 ] ))
							{
								index = 1;
								minRadius = newSphere[ 1 ].radius;
							}

							if( newSphere[ 2 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 2 ] ) &&
								CheckPointInSphere( points[ 1 ], newSphere[ 2 ] ))
							{
								index = 2;
								minRadius = newSphere[ 2 ].radius;
							}

							if( newSphere[ 3 ].radius < minRadius &&
								CheckPointInSphere( points[ 2 ], newSphere[ 3 ] ))
							{
								index = 3;
								minRadius = newSphere[ 3 ].radius;
							}

							if( newSphere[ 4 ].radius < minRadius &&
								CheckPointInSphere( points[ 1 ], newSphere[ 4 ] ))
							{
								index = 4;
								minRadius = newSphere[ 4 ].radius;
							}

							if( newSphere[ 5 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 5 ] ))
							{
								index = 5;
								minRadius = newSphere[ 5 ].radius;
							}

							if( index > -1 )
							{
								minSphere = newSphere[ index ];

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
								}
							}
							else
							{
								minSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ], points[ 2 ] );
								points.push_back( pos );
							}
							break;

						case 4:
							for( UINT y = 0; y < 4; ++y )
							{
								newSphere[ y ] = BoundingSphere( pos, points[ y ] );
							}

							for( UINT y = 4; y < 7; ++y )
							{
								newSphere[ y ] = BoundingSphere( pos, points[ 0 ], points[ y - 3 ] );
							}

							newSphere[ 7 ]  = BoundingSphere( pos, points[ 1 ], points[ 2 ] );
							newSphere[ 8 ]  = BoundingSphere( pos, points[ 1 ], points[ 3 ] );
							newSphere[ 9 ]  = BoundingSphere( pos, points[ 2 ], points[ 3 ] );
							newSphere[ 10 ] = BoundingSphere( pos, points[ 0 ], points[ 1 ], points[ 2 ] );
							newSphere[ 11 ] = BoundingSphere( pos, points[ 0 ], points[ 1 ], points[ 3 ] );
							newSphere[ 12 ] = BoundingSphere( pos, points[ 1 ], points[ 2 ], points[ 3 ] );
							newSphere[ 13 ] = BoundingSphere( pos, points[ 0 ], points[ 2 ], points[ 3 ] );

							// Check 2 point spheres.
							//
							if( CheckPointInSphere( points[ 1 ], newSphere[ 0 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 0 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 0 ] ))
							{
								index = 0;
								minRadius = newSphere[ 0 ].radius;
							}

							if( newSphere[ 1 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 1 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 1 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 1 ] ))
							{
								index = 1;
								minRadius = newSphere[ 1 ].radius;
							}

							if( newSphere[ 2 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 2 ] ) &&
								CheckPointInSphere( points[ 1 ], newSphere[ 2 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 2 ] ))
							{
								index = 2;
								minRadius = newSphere[ 2 ].radius;
							}

							if( newSphere[ 3 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 3 ] ) &&
								CheckPointInSphere( points[ 1 ], newSphere[ 3 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 3 ] ))
							{
								index = 3;
								minRadius = newSphere[ 3 ].radius;
							}

							// Check 3 point spheres.
							//
							if( newSphere[ 4 ].radius < minRadius &&
								CheckPointInSphere( points[ 2 ], newSphere[ 4 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 4 ] ))
							{
								index = 4;
								minRadius = newSphere[ 4 ].radius;
							}

							if( newSphere[ 5 ].radius < minRadius &&
								CheckPointInSphere( points[ 1 ], newSphere[ 5 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 5 ] ))
							{
								index = 5;
								minRadius = newSphere[ 5 ].radius;
							}

							if( newSphere[ 6 ].radius < minRadius &&
								CheckPointInSphere( points[ 1 ], newSphere[ 6 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 6 ] ))
							{
								index = 6;
								minRadius = newSphere[ 6 ].radius;
							}

							if( newSphere[ 7 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 7 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 7 ] ))
							{
								index = 7;
								minRadius = newSphere[ 7 ].radius;
							}

							if( newSphere[ 8 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 8 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 8 ] ))
							{
								index = 8;
								minRadius = newSphere[ 8 ].radius;
							}

							if( newSphere[ 9 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 9 ] ) &&
								CheckPointInSphere( points[ 1 ], newSphere[ 9 ] ))
							{
								index = 9;
								minRadius = newSphere[ 9 ].radius;
							}

							// Check 4 point spheres.
							//
							if( newSphere[ 10 ].radius < minRadius &&
								CheckPointInSphere( points[ 3 ], newSphere[ 10 ] ))
							{
								index = 10;
								minRadius = newSphere[ 10 ].radius;
							}

							if( newSphere[ 11 ].radius < minRadius &&
								CheckPointInSphere( points[ 2 ], newSphere[ 11 ] ))
							{
								index = 11;
								minRadius = newSphere[ 11 ].radius;
							}

							if( newSphere[ 12 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 12 ] ))
							{
								index = 12;
								minRadius = newSphere[ 12 ].radius;
							}

							if( newSphere[ 13 ].radius < minRadius &&
								CheckPointInSphere( points[ 1 ], newSphere[ 13 ] ))
							{
								index = 13;
								minRadius = newSphere[ 13 ].radius;
							}

							minSphere = newSphere[ index ];

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

					if( minSphere.radius > sphere.radius )
					{
						sphere = minSphere;
						currIndex = 0;
						continue;
					}
				}
			}

			++currIndex;
		}

		vbo->Unlock();

		return sphere;
	}
}
