#pragma once
/*
Provides complete customization of objects.

Always set mShader to a valid shader before calling BuildMesh()

Each Vertex contains all the possible variations of vertices
for ease of creation.  These vertices are then copied over
into another buffer of the shader specifications.
*/
#include <memory>
#include <vector>

#include "Common.h"
#include "Mesh.h"

namespace Sentinel
{
	class SENTINEL_DLL MeshBuilder
	{
		friend class Mesh;

	public:

		struct Vertex
		{
			Vector3f	mPosition;
			Vector2f	mTextureCoords[ 2 ];
			Vector3f	mNormal;
			UINT		mColor;

			// Normal Mapping.
			//
			Vector4f	mTangent;

			// Bones.
			//
			float		mWeight[ 4 ];
			int			mMatrixIndex[ 4 ];
			int			mNumBones;

			// Vertex Matrix for Sprites.
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

		Buffer*					mVBO;
		Buffer*					mIBO;

	public:

		Shader*					mShader;
		PrimitiveType			mPrimitive;

		Texture*				mTexture[ NUM_TEXTURES ];	// uses TextureType
		Vector4f				mTextureScale;

		std::vector< Vertex >	mVertex;
		std::vector< UINT >		mIndex;

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
		void	CreateQuad( float size, const Vector3f& normal = Vector3f( 0, 0, 1 ));
		void	CreateCube( float size );
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

		void	CreateBuffers();

	public:

		// Returns the mesh created from the buffers.
		// Ensure mShader is set before calling this.
		//
		Mesh*	BuildMesh();
	};
}
