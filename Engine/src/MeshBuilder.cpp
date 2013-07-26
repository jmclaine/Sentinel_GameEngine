#include <vector>
#include <memory>

#include "Renderer.h"
#include "MathCommon.h"
#include "MeshBuilder.h"

namespace Sentinel
{
	MeshBuilder::MeshBuilder()
	{
		ClearAll();
	}

	MeshBuilder::~MeshBuilder()
	{}

	void MeshBuilder::ClearAll()
	{
		mPrimitive = TRIANGLE_LIST;

		for( UINT i = 0; i < NUM_TEXTURES; ++i )
		{
			mTexture[ i ] = Renderer::Inst()->BASE_TEXTURE;
		}

		mVBO = NULL;
		mIBO = NULL;

		mVertex.clear();
		mIndex.clear();

		mTextureScale = Vector4f( 0, 0, 1, 1 );
	}

	void MeshBuilder::ClearGeometry()
	{
		mVertex.clear();
		mIndex.clear();
	}

	//////////////////////////////////////////////////////////////////
	
	UINT MeshBuilder::FindVertex( const Vector3f& pos, const Vector2f& tex, const Vector3f& normal )
	{
		for( UINT i = 0; i < mVertex.size(); ++i )
		{
			if( mVertex[ i ].mPosition == pos && mVertex[ i ].mTextureCoords[ 0 ] == tex && mVertex[ i ].mNormal == normal )
				return i;
		}

		return UINT_MAX;	// Vertex not found.
	}

	//////////////////////////////////////////////////////////////////
	
	void MeshBuilder::AddIndex( UINT i0 )
	{
		mIndex.push_back( i0 );
	}

	void MeshBuilder::AddIndex( UINT i0, UINT i1 )
	{
		AddIndex( i0 );
		AddIndex( i1 );
	}

	void MeshBuilder::AddIndex( UINT i0, UINT i1, UINT i2 )
	{
		AddIndex( i0 );
		AddIndex( i1 );
		AddIndex( i2 );
	}

	void MeshBuilder::AddIndex( UINT i0, UINT i1, UINT i2, UINT i3 )
	{
		AddIndex( i0, i1, i2 );
		AddIndex( i0, i2, i3 );
	}

	void MeshBuilder::AddIndex( UINT i0, UINT i1, UINT i2, UINT i3, UINT i4 )
	{
		AddIndex( i0, i1, i2 );
		AddIndex( i0, i2, i3 );
		AddIndex( i0, i3, i4 );
	}

	////////////////////////////////////////////////////

	void MeshBuilder::CalculateTangents( bool doNormals )
	{
		Vector3f* tangent  = new Vector3f[ mVertex.size() ];
		Vector3f* binormal = new Vector3f[ mVertex.size() ];
		Vector3f* normal	= new Vector3f[ mVertex.size() ];

		for( UINT i = 0; i < mIndex.size(); i += 3 )
		{
			UINT j[ 3 ] = { mIndex[i], mIndex[i+1], mIndex[i+2] };

			// Calculate texture coordinate directions.
			//
			Vector3f Q1 = mVertex[ j[1] ].mPosition - mVertex[ j[0] ].mPosition;
			Vector3f Q2 = mVertex[ j[2] ].mPosition - mVertex[ j[0] ].mPosition;

			// Get the normal.
			//
			Vector3f N;
			if( doNormals )
			{
				N = (Q1.Cross( Q2 )).Normalize();

				mVertex[ j[0] ].mNormal = N;
				mVertex[ j[1] ].mNormal = N;
				mVertex[ j[2] ].mNormal = N;
			}
			else
			{
				N = (mVertex[ j[0] ].mNormal).Normalize();
			}

			// Calculate the tangents.
			//
			float s1 = mVertex[ j[1] ].mTextureCoords[0].X() - mVertex[ j[0] ].mTextureCoords[0].X();
			float t1 = mVertex[ j[1] ].mTextureCoords[0].Y() - mVertex[ j[0] ].mTextureCoords[0].Y();

			float s2 = mVertex[ j[2] ].mTextureCoords[0].X() - mVertex[ j[0] ].mTextureCoords[0].X();
			float t2 = mVertex[ j[2] ].mTextureCoords[0].Y() - mVertex[ j[0] ].mTextureCoords[0].Y();

			float det = 1.0f / (s1*t2 - s2*t1);

			Vector3f T = Vector3f( (Q1.X() * t2 + Q2.X() * -t1) * det, \
							 (Q1.Y() * t2 + Q2.Y() * -t1) * det, \
							 (Q1.Z() * t2 + Q2.Z() * -t1) * det ).Normalize();

			Vector3f B = Vector3f( (Q1.X() * -s2 + Q2.X() * s1) * det, \
							 (Q1.Y() * -s2 + Q2.Y() * s1) * det, \
							 (Q1.Z() * -s2 + Q2.Z() * s1) * det ).Normalize();

			// Sum the tangents up by their vertex for averaging.
			//
			for( UINT k = 0; k < 3; ++k )
			{
				tangent[ j[k] ]  += T;
				binormal[ j[k] ] += B;
				normal[ j[k] ]   += N;
			}
		}

		// Calculate the average tangent.
		//
		for( UINT i = 0; i < mVertex.size(); ++i )
		{
			// Find the tangent.
			//
			Vector3f Tn = (tangent[ i ] - normal[ i ] * normal[ i ].Dot( tangent[ i ] )).Normalize();

			// Find the binormal.
			//
			Vector3f Bn = (binormal[ i ] - normal[ i ] * normal[ i ].Dot( binormal[ i ] ) - Tn * Tn.Dot( binormal[ i ] )).Normalize();

			// Determine the sign.
			//
			Vector3f Bt = (normal[ i ].Cross(Tn)).Normalize();
			float sign = (Bt.Dot( Bn ) > 0.0f) ? 1.0f : -1.0f;

			// Finalize the tangent as the average.
			//
			mVertex[ i ].mTangent = Vector4f( Tn.X(), Tn.Y(), Tn.Z(), sign );
		}

		delete[] tangent;
		delete[] binormal;
		delete[] normal;
	}

	//////////////////////////////////////////////////////////////////
	
	// Create a quad based on a normal and size.
	//
	void MeshBuilder::CreateQuad( float size, const Vector3f& pos, const Vector3f& normal )
	{
		UINT startVert = mVertex.size();

		Vector3f up = Vector3f( normal.Y(), normal.Z(), normal.X() );
		Vector3f right = normal.Cross( up ).Normalize();

		Vector3f cornerUR = right + up;
		Vector3f cornerUL = up - right;

		Vertex vertex;
		vertex.mPosition			= cornerUL * -size + pos;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0, 1 );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0, 1 );
		vertex.mNormal				= normal;

		mVertex.push_back( vertex );
		
		vertex.mPosition			= cornerUR * -size + pos;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1, 1 );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1, 1 );
		vertex.mNormal				= normal;

		mVertex.push_back( vertex );

		vertex.mPosition			= cornerUL * size + pos;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1, 0 );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1, 0 );
		vertex.mNormal				= normal;

		mVertex.push_back( vertex );

		vertex.mPosition			= cornerUR * size + pos;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0, 0 );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0, 0 );
		vertex.mNormal				= normal;

		mVertex.push_back( vertex );

		AddIndex( startVert, startVert+1, startVert+2, startVert+3 );

		mPrimitive = TRIANGLE_LIST;
	}

#define CUBE_SIDE( n )\
	normal = n;\
	newPos = pos + normal * extSize;\
	CreateQuad( size, newPos, normal );

	void MeshBuilder::CreateCube( float size, const Vector3f& pos )
	{
		Vector3f newPos;
		Vector3f normal;
		float extSize = size;

		// Front
		CUBE_SIDE( Vector3f( 0, 0, -1 ));

		// Left
		CUBE_SIDE( Vector3f( -1, 0, 0 ));

		// Bottom
		CUBE_SIDE( Vector3f( 0, -1, 0 ));

		// Back
		CUBE_SIDE( Vector3f( 0, 0, 1 ));

		// Right
		CUBE_SIDE( Vector3f( 1, 0, 0 ));

		// Top
		CUBE_SIDE( Vector3f( 0, 1, 0 ));
	}

	// Shape code base from geometry.h of OpenGL.
	// Creates a lookup table for drawing a circle.
	//
	void CircleTable( float **sint, float **cost, const int n )
	{
		int i;

		/* Table size, the sign of n flips the circle direction */

		const int size = abs( n );

		/* Determine the angle between samples */

		const float angle = (float)(2 * PI) / (float)n;

		/* Allocate memory for n samples, plus duplicate of first entry at the end */

		*sint = (float*)calloc( sizeof(double), size+1 );
		*cost = (float*)calloc( sizeof(double), size+1 );

		/* Bail out if memory allocation fails */

		if( !(*sint) || !(*cost) )
		{
			free( *sint );
			free( *cost );
		}

		/* Compute cos and sin around the circle */

		for( i = 0; i < size; i++ )
		{
			(*sint)[ i ] = ::sin( angle*i );
			(*cost)[ i ] = ::cos( angle*i );
		}

		/* Last sample is duplicate of the first */

		(*sint)[ size ] = (*sint)[ 0 ];
		(*cost)[ size ] = (*cost)[ 0 ];
	}

	void MeshBuilder::CreateCylinder( float radius, float height, int slices, int stacks )
	{
		int i,j;

		/* Step in z and radius as stacks are drawn. */

		float z0, z1;
		const float zStep   = height / stacks;
		const float zHeight = height / 2.0f;

		/* Pre-computed circle */

		float *sint, *cost;
		CircleTable( &sint, &cost, -slices );

		/* Cover the base and top */

		UINT startVertex = mVertex.size();
		int vertCount = (int)startVertex;

		Vertex vertex;
		vertex.mPosition			= Vector3f( 0.0f, -zHeight, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.5f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.5f );
		vertex.mNormal				= Vector3f( 0.0f, -1.0f, 0.0f );

		mVertex.push_back( vertex );

		for( j = 0; j <= slices; j++ )
		{
			vertex.mPosition			= Vector3f( cost[ j ]*radius, -zHeight, sint[ j ]*radius );
			vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f + cost[ j ]*0.5f, 0.5f + sint[ j ]*0.5f );
			vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f + cost[ j ]*0.5f, 0.5f + sint[ j ]*0.5f );

			mVertex.push_back( vertex );
		}

		for( j = vertCount; j < (int)mVertex.size()-2; j++ )
		{
			AddIndex( vertCount, j+2, j+1 );
		}

		/////////////////////////////

		vertCount = (int)mVertex.size();

		vertex.mPosition			= Vector3f( 0.0f, zHeight, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.5f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.5f );
		vertex.mNormal				= Vector3f( 0.0f, 1.0f, 0.0f );

		mVertex.push_back( vertex );

		for( j = slices; j >= 0; j-- )
		{
			vertex.mPosition			= Vector3f( cost[ j ]*radius, zHeight, sint[ j ]*radius );
			vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f + cost[ j ]*0.5f, 0.5f + sint[ j ]*0.5f );
			vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f + cost[ j ]*0.5f, 0.5f + sint[ j ]*0.5f );

			mVertex.push_back( vertex );
		}

		for( j = vertCount; j < (int)mVertex.size()-2; j++ )
		{
			AddIndex( vertCount, j+2, j+1 );
		}
		
		/* Do the stacks */

		z0 = -zHeight;
		z1 = zStep;

		for( i = 1; i <= stacks; i++ )
		{
			if( i == stacks )
			{
				z1 = zHeight;
			}

			vertCount = (int)mVertex.size();

			for( j = 0; j <= slices; j++ )
			{
				float u = ((float)j / (float)slices * 2.0f);
				
				vertex.mPosition			= Vector3f( cost[ j ]*radius, z0, sint[ j ]*radius );
				vertex.mTextureCoords[ 0 ]	= Vector2f( u, 0.0f );
				vertex.mTextureCoords[ 1 ]	= Vector2f( u, 0.0f );
				vertex.mNormal				= Vector3f( cost[ j ], 0.0f, sint[ j ] );

				mVertex.push_back( vertex );

				vertex.mPosition			= Vector3f( cost[ j ]*radius, z1, sint[ j ]*radius );
				vertex.mTextureCoords[ 0 ]	= Vector2f( u, 1.0f );
				vertex.mTextureCoords[ 1 ]	= Vector2f( u, 1.0f );
				vertex.mNormal				= Vector3f( cost[ j ], 0.0f, sint[ j ] );

				mVertex.push_back( vertex );
			}
			
			for( j = vertCount; j < (int)mVertex.size()-3; j+=2 )
			{
				AddIndex( j, j+2, j+3, j+1 );
			}

			z0 = z1;
			z1 += zStep;
		}

		/* Release sin and cos tables */

		free( sint );
		free( cost );

		mPrimitive = TRIANGLE_LIST;
	}

	void MeshBuilder::CreateTetrahedron( float scale )
	{
		/* Magic Numbers:  r0 = ( 1, 0, 0 )
		*                  r1 = ( -1/3, 2 sqrt(2) / 3, 0 )
		*                  r2 = ( -1/3, -sqrt(2) / 3, sqrt(6) / 3 )
		*                  r3 = ( -1/3, -sqrt(2) / 3, -sqrt(6) / 3 )
		* |r0| = |r1| = |r2| = |r3| = 1
		* Distance between any two points is 2 sqrt(6) / 3
		*
		* Normals:  The unit normals are simply the negative of the coordinates of the point not on the surface.
		*/

		Vector3f r0(             1.0f,				0.0f,			  0.0f );
		Vector3f r1( -0.333333333333f,  0.942809041582f,			  0.0f );
		Vector3f r2( -0.333333333333f, -0.471404520791f,  0.816496580928f );
		Vector3f r3( -0.333333333333f, -0.471404520791f, -0.816496580928f );

		scale *= 1.25f;
		r0 *= scale;
		r1 *= scale;
		r2 *= scale;
		r3 *= scale;

		UINT numVerts = mVertex.size();

		////////////////////////////////////////

		Vertex vertex;
		vertex.mPosition			= r1;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 1.0f );
		vertex.mNormal				= Vector3f( -1.0f, 0.0f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= r3;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( -1.0f, 0.0f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= r2;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( -1.0f, 0.0f, 0.0f );

		mVertex.push_back( vertex );

		////////////////////////////////////////

		vertex.mPosition			= r0;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 1.0f );
		vertex.mNormal				= Vector3f( 0.333333333333f, -0.942809041582f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= r2;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.333333333333f, -0.942809041582f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= r3;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.333333333333f, -0.942809041582f, 0.0f );

		mVertex.push_back( vertex );

		////////////////////////////////////////
		
		vertex.mPosition			= r0;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 1.0f );
		vertex.mNormal				= Vector3f( 0.333333333333f,  0.471404520791f, -0.816496580928f );

		mVertex.push_back( vertex );

		vertex.mPosition			= r3;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.333333333333f,  0.471404520791f, -0.816496580928f );

		mVertex.push_back( vertex );

		vertex.mPosition			= r1;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.333333333333f,  0.471404520791f, -0.816496580928f );

		mVertex.push_back( vertex );

		////////////////////////////////////////
		
		vertex.mPosition			= r0;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 1.0f );
		vertex.mNormal				= Vector3f( 0.333333333333f,  0.471404520791f,  0.816496580928f );

		mVertex.push_back( vertex );

		vertex.mPosition			= r1;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.333333333333f,  0.471404520791f,  0.816496580928f );

		mVertex.push_back( vertex );

		vertex.mPosition			= r2;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.333333333333f,  0.471404520791f,  0.816496580928f );

		mVertex.push_back( vertex );

		////////////////////////////////////////

		for( UINT x = numVerts; x < mVertex.size(); x += 3 )
		{
			AddIndex( x, x+1, x+2 );
		}

		mPrimitive = TRIANGLE_LIST;
	}

	void MeshBuilder::CreateOctahedron( float radius )
	{
		UINT numVerts = mVertex.size();

		////////////////////////////////////////

		Vertex vertex;
		vertex.mPosition			= Vector3f( radius, 0.0f, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, 0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, radius, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, 0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 0.0f, radius );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, 0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		////////////////////////////////////////

		vertex.mPosition			= Vector3f( radius, 0.0f, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, 0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 0.0f, -radius );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, 0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, radius, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, 0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		//////////////////////////////////////////

		vertex.mPosition			= Vector3f( radius, 0.0f, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, -0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 0.0f, radius );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, -0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, -radius, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, -0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		//////////////////////////////////////////

		vertex.mPosition			= Vector3f( radius, 0.0f, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, -0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, -radius, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, -0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 0.0f, -radius );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( 0.577350269189f, -0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		//////////////////////////////////////////

		vertex.mPosition			= Vector3f( -radius, 0.0f, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, 0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 0.0f, radius );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, 0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, radius, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, 0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		//////////////////////////////////////////

		vertex.mPosition			= Vector3f( -radius, 0.0f, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, 0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, radius, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, 0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 0.0f, -radius );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, 0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		//////////////////////////////////////////

		vertex.mPosition			= Vector3f( -radius, 0.0f, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, -0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f,-radius, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, -0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 0.0f, radius );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, -0.577350269189f, 0.577350269189f );

		mVertex.push_back( vertex );

		//////////////////////////////////////////

		vertex.mPosition			= Vector3f( -radius, 0.0f, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, -0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 0.0f, -radius );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 1.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, -0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, -radius, 0.0f );
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.0f );
		vertex.mNormal				= Vector3f( -0.577350269189f, -0.577350269189f, -0.577350269189f );

		mVertex.push_back( vertex );

		////////////////////////////////////////

		for( UINT x = numVerts; x < mVertex.size(); x += 3 )
		{
			AddIndex( x, x+1, x+2 );
		}

		mPrimitive = TRIANGLE_LIST;
	}

	void MeshBuilder::CreateDodecahedron( float scale )
	{
		UINT numVerts = mVertex.size();

		scale *= 0.625f;

		Vertex vertex;
		vertex.mPosition			= Vector3f( 0.0f, 1.61803398875f, 0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, 1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -0.61803398875f, 0.0f, 1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.61803398875f, 0.0f, 1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, 1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( 0.0f, 1.61803398875f, -0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, 1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.61803398875f, 0.0f, -1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -0.61803398875f, 0.0f, -1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, 1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.0f, 0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( 0.0f, -1.61803398875f, 0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, -1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.61803398875f, 0.0f, 1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -0.61803398875f, 0.0f, 1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, -1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, 0.850650808354f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( 0.0f, -1.61803398875f, -0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, -1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -0.61803398875f, 0.0f, -1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.61803398875f, 0.0f, -1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, -1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.0f, -0.525731112119f, -0.850650808354f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( 0.61803398875f, 0.0f, 1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, -1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.61803398875f, -0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.61803398875f, 0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, 1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( -0.61803398875f, 0.0f, 1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, 1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.61803398875f, 0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.61803398875f, -0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, -1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, 0.525731112119f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( 0.61803398875f, 0.0f, -1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, 1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.61803398875f, 0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.61803398875f, -0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, -1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( -0.61803398875f, 0.0f, -1.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, -1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.61803398875f, -0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.61803398875f, 0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, 1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( -0.850650808354f, 0.0f, -0.525731112119f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( 1.61803398875f, 0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( 0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, 1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 1.61803398875f, -0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( 0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 1.61803398875f, 0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( 0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, 1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( 1.61803398875f, -0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( 0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, -1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, -1.61803398875f, 0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( 0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, -1.61803398875f, -0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( 0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 1.0f, -1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( 0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( -1.61803398875f, 0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( -0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, 1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( -0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 1.61803398875f, 0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( -0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, 1.61803398875f, -0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( -0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, 1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( -0.525731112119f, 0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////

		vertex.mPosition			= Vector3f( -1.61803398875f, -0.61803398875f, 0.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.5f, 0.0f );
		vertex.mNormal				= Vector3f( -0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, -1.0f, -1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.0f, 0.31f );
		vertex.mNormal				= Vector3f( -0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, -1.61803398875f, -0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.31f, 1.0f );
		vertex.mNormal				= Vector3f( -0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( 0.0f, -1.61803398875f, 0.61803398875f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 0.69f, 1.0f );
		vertex.mNormal				= Vector3f( -0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		vertex.mPosition			= Vector3f( -1.0f, -1.0f, 1.0f ) * scale;
		vertex.mTextureCoords[ 0 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mTextureCoords[ 1 ]	= Vector2f( 1.0f, 0.31f );
		vertex.mNormal				= Vector3f( -0.525731112119f, -0.850650808354f, 0.0f );

		mVertex.push_back( vertex );

		/////////////////////////////////////////////////////////////////
		
		for( UINT x = numVerts; x < mVertex.size(); x += 5 )
		{
			AddIndex( x, x+1, x+2, x+3, x+4 );
		}

		mPrimitive = TRIANGLE_LIST;
	}

	void MeshBuilder::CreateWireSphere( float radius, int slices, int stacks )
	{
		Vertex vertex;

		int i, j;

		/* Adjust z and radius as stacks and slices are drawn. */

		float r;
		float x, y, z;

		/* Pre-computed circle */

		float *sint1, *cost1;
		float *sint2, *cost2;
		CircleTable( &sint1, &cost1, -slices );
		CircleTable( &sint2, &cost2, stacks<<1 );

		/* Draw a line loop for each stack */

		for( i = 1; i < stacks; i++ )
		{
			int vertCount = mVertex.size();
			z = cost2[ i ];
			r = sint2[ i ];

			for( j = 0; j <= slices; j++ )
			{
				x = cost1[ j ];
				y = sint1[ j ];

				vertex.mPosition			= Vector3f( x*r*radius, y*r*radius, z*radius );
				vertex.mNormal				= Vector3f( x, y, z );

				mVertex.push_back( vertex );
			}

			vertex.mPosition = mVertex[ 0 ].mPosition;

			mVertex.push_back( vertex );

			for( j = vertCount; j < (int)mVertex.size()-2; j++ )
			{
				AddIndex( j, j+1 );
			}
		}

		/* Draw a line loop for each slice */

		for( i = 0; i < slices; i++ )
		{
			int vertCount = (int)mVertex.size();

			for( j = 0; j <= stacks; j++ )
			{
				x = cost1[ i ] * sint2[ j ];
				y = sint1[ i ] * sint2[ j ];
				z = cost2[ j ];

				vertex.mPosition			= Vector3f( x*radius, y*radius, z*radius );
				vertex.mNormal				= Vector3f( x, y, z );

				mVertex.push_back( vertex );
			}

			for( j = vertCount; j < (int)mVertex.size()-1; j++ )
			{
				AddIndex( j, j+1 );
			}
		}

		/* Release sin and cos tables */

		free( sint1 );
		free( cost1 );
		free( sint2 );
		free( cost2 );

		mPrimitive = LINE_LIST;
	}

	void MeshBuilder::CreateSphere( float radius, int slices, int stacks, int texWrap )
	{
		int i, j;

		/* Adjust z and radius as stacks are drawn. */

		float z0, z1;
		float r0, r1;

		/* Pre-computed circle */

		float *sint1, *cost1;
		float *sint2, *cost2;
		CircleTable( &sint1, &cost1, -slices );
		CircleTable( &sint2, &cost2, stacks<<1 );

		float u, v;
		float tx, ty;
		float tx1, ty1;

		/* The top stack is covered with a triangle fan */

		z0 = 1.0; z1 = cost2[ 1 ];
		r0 = 0.0; r1 = sint2[ 1 ];

		int vertCount = (int)mVertex.size();

		Vertex vertex;
		vertex.mPosition			= Vector3f( 0.0f, 0.0f, radius );
		vertex.mNormal				= Vector3f( 0.0f, 0.0f, 1.0f );

		tx1 = 0.0f; //(atan2(0.0f, 1.0f) / (2.0f * (float)PI) + 0.5f) * texWrap;
		ty1 = 0.0f; //(acos(0.0f) / (float)PI + 0.5f) * texWrap;
		vertex.mTextureCoords[ 0 ]	= Vector2f( tx1, ty1 );
		vertex.mTextureCoords[ 1 ]	= Vector2f( tx1, ty1 );

		mVertex.push_back( vertex );
		
		for( j = slices; j >= 0; j-- )
		{
			u = cost1[ j ]*r1;
			v = sint1[ j ]*r1;
			vertex.mPosition			= Vector3f( u*radius, v*radius, z1*radius );
			vertex.mNormal				= Vector3f( u, v, z1 );

			tx1 = u * texWrap; //(atan2(u, z1) / (2.0f * (float)PI) + 0.5f) * texWrap;
			ty1 = v * texWrap; //(acos(v) / (float)PI + 0.5f) * texWrap;
			vertex.mTextureCoords[ 0 ]	= Vector2f( tx1, ty1 );
			vertex.mTextureCoords[ 1 ]	= Vector2f( tx1, ty1 );

			mVertex.push_back( vertex );
		}

		for( j = vertCount; j < (int)mVertex.size()-2; j++ )
		{
			AddIndex( vertCount, j+1, j+2 );
		}

		/* Cover each stack with a quad strip, except the top and bottom stacks */

		for( i = 1; i < stacks-1; i++ )
		{
			vertCount = (int)mVertex.size();

			z0 = z1; z1 = cost2[ i+1 ];
			r0 = r1; r1 = sint2[ i+1 ];

			for( j = 0; j <= slices; j++ )
			{
				u = cost1[ j ]*r1;
				v = sint1[ j ]*r1;

				vertex.mPosition			= Vector3f( u*radius, v*radius, z1*radius );
				vertex.mNormal				= Vector3f( u, v, z1 );

				tx = u * texWrap; //(atan2(u, z1) / (2.0f * (float)PI) + 0.5f) * texWrap;
				ty = v * texWrap; //(acos(v) / (float)PI + 0.5f) * texWrap;
				//clamp( tx, -2.0f*(float)PI, 2.0f*(float)PI );
				//clamp( ty, -2.0f*(float)PI, 2.0f*(float)PI );
				vertex.mTextureCoords[ 0 ]	= Vector2f( tx, ty );
				vertex.mTextureCoords[ 1 ]	= Vector2f( tx, ty );

				mVertex.push_back( vertex );
				
				///////////////////////////////////////////////

				u = cost1[ j ]*r0;
				v = sint1[ j ]*r0;
				vertex.mPosition			= Vector3f( u*radius, v*radius, z0*radius );
				vertex.mNormal				= Vector3f( u, v, z0 );

				tx = u * texWrap; //(atan2(u, z1) / (2.0f * (float)PI) + 0.5f) * texWrap;
				ty = v * texWrap; //(acos(v) / (float)PI + 0.5f) * texWrap;
				//clamp( tx, -2.0f*(float)PI, 2.0f*(float)PI );
				//clamp( ty, -2.0f*(float)PI, 2.0f*(float)PI );
				vertex.mTextureCoords[ 0 ]	= Vector2f( tx, ty );
				vertex.mTextureCoords[ 1 ]	= Vector2f( tx, ty );

				mVertex.push_back( vertex );
			}

			for( j = vertCount; j < (int)mVertex.size()-3; j+=2 )
			{
				AddIndex( j, j+1, j+3, j+2 );
			}
		}

		///* The bottom stack is covered with a triangle fan */

		z0 = z1;
		r0 = r1;

		vertCount = (int)mVertex.size();

		vertex.mPosition			= Vector3f( 0.0f, 0.0f, -radius );
		vertex.mNormal				= Vector3f( 0.0f, 0.0f, -1.0f );

		tx1 = 0.0f; //(atan2(0.0f, -1.0f) / (2.0f * (float)PI) + 0.5f) * texWrap;
		ty1 = 0.0f; //(acos(0.0f) / (float)PI + 0.5f) * texWrap;
		vertex.mTextureCoords[ 0 ]	= Vector2f( tx1, ty1 );
		vertex.mTextureCoords[ 1 ]	= Vector2f( tx1, ty1 );

		mVertex.push_back( vertex );

		for( j = 0; j <= slices; j++ )
		{
			u = cost1[ j ]*r0;
			v = sint1[ j ]*r0;
			vertex.mPosition			= Vector3f( u*radius, v*radius, z0*radius );
			vertex.mNormal				= Vector3f( u, v, z0 );
			
			tx1 = u * texWrap; //(atan2(u, z1) / (2.0f * (float)PI) + 0.5f) * texWrap;
			ty1 = v * texWrap; //(acos(v) / (float)PI + 0.5f) * texWrap;
			vertex.mTextureCoords[ 0 ]	= Vector2f( tx1, ty1 );
			vertex.mTextureCoords[ 1 ]	= Vector2f( tx1, ty1 );

			mVertex.push_back( vertex );
		}
		
		for( j = vertCount; j < (int)mVertex.size()-2; j++ )
		{
			AddIndex( vertCount, j+1, j+2 );
		}

		/* Release sin and cos tables */

		free( sint1 );
		free( cost1 );
		free( sint2 );
		free( cost2 );

		mPrimitive = TRIANGLE_LIST;
	}

	// Apply a matrix transform to a set a vertices.
	//
	void MeshBuilder::ApplyMatrix( const Matrix4f& mat, UINT startVertex, UINT endVertex )
	{
		if( endVertex == UINT_MAX )
			endVertex = mVertex.size();

		for( UINT x = startVertex; x < endVertex; ++x )
		{
			mVertex[ x ].mPosition = mat.Transform( mVertex[ x ].mPosition );
		}
	}

	// Create the VBO and IBO.
	//
	void MeshBuilder::CreateBuffers()
	{
		_ASSERT( mVertex.size() > 0 );
		_ASSERT( mIndex.size() > 0 );

		std::string attrib	= mShader->AttributeDecl();
		UINT vertexSize		= mShader->VertexSize();

		UINT numVerts		= mVertex.size();
		UINT totalSize		= vertexSize * numVerts;

		// Copy the generic vertex into a custom vertex using memory management.
		//
		void* memBlock;
		memBlock = malloc( totalSize );
		UCHAR* base = (UCHAR*)memBlock;

		for( UINT j = 0; j < numVerts; ++j )
		{
			for( UINT i = 0; i < attrib.size(); ++i )
			{
				switch( attrib[ i ] )
				{
				// Position.
				//
				case 'P':
					for( UINT k = 0; k < 3; ++k )
					{
						*((float*)base) = mVertex[ j ].mPosition[ k ];
						base += sizeof(float);
					}
					break;

				// Texture Coordinate.
				//
				case 'X':
					for( UINT k = 0; k < 2; ++k )
					{
						*((float*)base) = mVertex[ j ].mTextureCoords[ 0 ][ k ];
						base += sizeof(float);
					}
					break;

				// Vertex Matrix.
				//
				case 'x':
					for( UINT k = 0; k < 16; ++k )
					{
						*((float*)base) = mVertex[ j ].mMatrixVertex.Get( k );
						base += sizeof(float);
					}
					break;

				// Normal.
				//
				case 'N':
					for( UINT k = 0; k < 3; ++k )
					{
						*((float*)base) = mVertex[ j ].mNormal[ k ];
						base += sizeof(float);
					}
					break;

				// Color.
				//
				case 'C':
					*((UINT*)base) = mVertex[ j ].mColor;
					base += sizeof(UINT);
					break;

				// Tangent.
				//
				case 'T':
					for( UINT k = 0; k < 4; ++k )
					{
						*((float*)base) = mVertex[ j ].mTangent[ k ];
						base += sizeof(float);
					}
					break;

				// Bones.
				//
				case 'B':
					// Number of bones.
					//
					*((int*)base) = mVertex[ j ].mNumBones;
					base += sizeof(int);

					// Bone index.
					//
					for( UINT k = 0; k < 4; ++k )
					{
						*((int*)base) = mVertex[ j ].mMatrixIndex[ k ];
						base += sizeof(int);
					}

					// Weights.
					//
					for( UINT k = 0; k < 4; ++k )
					{
						*((float*)base) = mVertex[ j ].mWeight[ k ];
						base += sizeof(float);
					}
					break;
				}
			}
		}

		mVBO = Renderer::Inst()->CreateBuffer( memBlock, totalSize, mShader->VertexSize(), VERTEX_BUFFER );
		mIBO = Renderer::Inst()->CreateBuffer( &mIndex.front(), mIndex.size()*sizeof(UINT), sizeof(UINT), INDEX_BUFFER );

		free( memBlock );
	}

	// Returns the mesh created from the buffers; otherwise, NULL.
	//
	Mesh* MeshBuilder::BuildMesh()
	{
		Mesh* mesh = new Mesh();
		
		CreateBuffers();

		mesh->mVBO = mVBO;
		mesh->mIBO = mIBO;

		for( UINT x = 0; x < NUM_TEXTURES; ++x )
		{
			mesh->mTexture[ x ] = mTexture[ x ];
		}

		mesh->mShader		= mShader;
		mesh->mPrimitive	= mPrimitive;
		mesh->mTextureScale = mTextureScale;

		return mesh;
	}
}
