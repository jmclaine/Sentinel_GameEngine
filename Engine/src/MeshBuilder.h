#pragma once

#include <memory>
#include <vector>

#include "Mesh.h"

namespace Sentinel
{
	class MeshBuilder
	{
	public:

		struct Vertex
		{
			vec3f mPosition;
			vec2f mTextureCoords[ 2 ];
			vec3f mNormal;
			UINT  mColor;

			// Normal Mapping.
			//
			vec4f mTangent;

			// Bones.
			//
			float mWeight[ 4 ];
			int   mMatrixIndex[ 4 ];
			int   mNumBones;

			// Vertex Matrix for Sprites.
			//
			mat4f mMatrixVertex;

			Vertex() :
				mPosition( vec3f( 0, 0, 0 )),
				mColor( 0xFFFFFFFF )
			{
				mMatrixVertex.Identity();
			}

			Vertex( const vec3f& pos, const ColorRGBA& color = ColorRGBA( 1, 1, 1, 1 )) :
				mPosition( pos ),
				mColor( COLORtoUINT( color.x, color.y, color.z, color.w ))
			{
				mMatrixVertex.Identity();
			}
		};

	public:

		Shader*					mShader;
		PrimitiveType			mPrimitive;

		Buffer*					mVBO;
		Buffer*					mIBO;

		Material				mMaterial;

		Texture*				mTexture[ NUM_TEXTURES ];
		vec4f					mTextureScale;

		std::vector< Vertex >	mVertex;
		std::vector< UINT >		mIndex;

		//////////////////////////////

		MeshBuilder();
		~MeshBuilder();

		void ClearAll();
		void ClearGeometry();

		// Returns UINT_MAX if not found.
		//
		UINT FindVertex( const vec3f& pos, const vec2f& tex, const vec3f& normal );

		// Index helper functions.
		//
		void AddIndex( UINT i0 );										// Point
		void AddIndex( UINT i0, UINT i1 );								// Line
		void AddIndex( UINT i0, UINT i1, UINT i2 );						// Triangle
		void AddIndex( UINT i0, UINT i1, UINT i2, UINT i3 );			// Quad
		void AddIndex( UINT i0, UINT i1, UINT i2, UINT i3, UINT i4 );	// Polygon

		// Tangents.
		// Call this function only after all vertices have been added.
		//
		void CalculateTangents( bool doNormals = false );

		// Create objects.
		//
		void CreateQuad( float size, const vec3f& pos = vec3f( 0, 0, 0 ), const vec3f& normal = vec3f( 0, 0, 1 ));
		void CreateCube( float size, const vec3f& pos = vec3f( 0, 0, 0 ));
		void CreateCylinder( float radius, float height, int slices, int stacks = 1 );
		void CreateTetrahedron( float scale );
		void CreateOctahedron( float radius );
		void CreateDodecahedron( float scale );
		void CreateWireSphere( float radius, int slices, int stacks );
		void CreateSphere( float radius, int slices, int stacks, int texWrap = 1 );

		// Apply a matrix transform to a set a vertices.
		//
		void ApplyMatrix( mat4f& mat, UINT startVertex = 0, UINT endVertex = UINT_MAX );

	private:

		void CreateBuffers();

	public:

		// Returns the mesh created from the buffers.
		// Ensure mShader is set before calling this.
		//
		Mesh* BuildMesh();
	};
}
