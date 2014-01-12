#pragma once
/*
Provides complete customization of objects.

Always set mShader to a valid shader before calling BuildMesh()

Each Vertex contains all the possible variations of vertices
for ease of creation.  These vertices are then copied over
into another buffer of the shader specifications.


TODO:

Create an FVF style format for the Vertex for memory optimization.
Would require making mShader private, and creating functions
to set and get the shader to ensure the Vertex is created with
the correct amount of memory before adding them to the list.
*/
#include <memory>
#include <vector>

#include "Common.h"
#include "RendererTypes.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix4f.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class Mesh;
	class Buffer;
	class Texture;
	class Shader;
	class Renderer;

	class SENTINEL_DLL MeshBuilder
	{
		friend class Mesh;

	public:

		// TODO: Create a FVF style Vertex instead of allocating
		//       the maximum amount of memory each time.
		//
		struct Vertex
		{
			Vector3f	mPosition;	// P

			// Texture Coordinates are either uv / xy based (X),
			// or xyzw / quad (x) based depending on the shader
			// attribute.
			//
			Vector2f	mTextureCoords[ NUM_TEXTURES ];	// X
			Vector4f	mQuadCoords[ NUM_TEXTURES ];	// x
			
			Vector3f	mNormal;	// N
			UINT		mColor;		// C

			// Normal Mapping.
			//
			Vector4f	mTangent;	// T

			// Bones.				// B
			//
			float		mWeight[ 4 ];
			int			mMatrixIndex[ 4 ];
			int			mNumBones;

			// Vertex Matrix for Sprites. // M
			//
			Matrix4f	mMatrixVertex;

			Vertex() :
				mPosition( Vector3f( 0, 0, 0 )),
				mColor( 0xFFFFFFFF )
			{
				mMatrixVertex.Identity();
			}

			Vertex( const Vector3f& pos, const ColorRGBA& color = ColorRGBA( 1, 1, 1, 1 )) :
				mPosition( pos )
			{
				mColor = color.ToUINT();

				mMatrixVertex.Identity();
			}
		};

	private:

		Buffer*						mVBO;
		Buffer*						mIBO;

	public:

		std::shared_ptr< Shader >	mShader;
		PrimitiveType				mPrimitive;

		std::shared_ptr< Texture >	mTexture[ NUM_TEXTURES ];	// uses TextureType
		Vector4f					mTextureScale;

		std::vector< Vertex >		mVertex;
		std::vector< UINT >			mIndex;

		//////////////////////////////

		MeshBuilder();
		~MeshBuilder();

		void	ClearAll();
		void	ClearGeometry();

		// Returns UINT_MAX if not found.
		// Helper function for model loaders.
		//
		UINT	FindVertex( const Vector3f& pos, const Vector2f& tex, const Vector3f& normal );
		
		// Index helper functions.
		//
		void	AddIndex( UINT i0 );										// Point
		void	AddIndex( UINT i0, UINT i1 );								// Line
		void	AddIndex( UINT i0, UINT i1, UINT i2 );						// Triangle
		void	AddIndex( UINT i0, UINT i1, UINT i2, UINT i3 );				// Quad
		void	AddIndex( UINT i0, UINT i1, UINT i2, UINT i3, UINT i4 );	// Polygon
		
		// Call this function only after all vertices have been added.
		// Used for normal mapping.
		//
		void	CalculateTangents( bool doNormals = false );

		// Create objects.
		// Based on geometry.h provided with OpenGL.
		//
		void	CreateLine( const Vector3f& start, const Vector3f& end );
		void	CreateQuad( float size, const Vector3f& normal = Vector3f( 0, 0, 1 ));
		void	CreateCube( float size );
		void	CreateWireCube( float size );
		void	CreateCylinder( float radius, float height, int slices, int stacks = 1 );
		void	CreateTetrahedron( float scale );
		void	CreateOctahedron( float radius );
		void	CreateDodecahedron( float scale );
		void	CreateWireSphere( float radius, int slices, int stacks );
		void	CreateSphere( float radius, int slices, int stacks, int texWrap = 1 );

		// Apply a matrix transform to a set a vertices.
		//
		void	ApplyMatrix( const Matrix4f& mat, UINT startVertex = 0, UINT endVertex = UINT_MAX );

	private:

		void	CreateBuffers( Renderer* renderer );

	public:

		// Returns the mesh created from the buffers.
		// Ensure mShader is set before calling this.
		//
		Mesh*	BuildMesh( Renderer* renderer );
	};
}
