#pragma once
/*
Contains all the functionality of the C++ version.
*/
#include "MeshBuilder.h"
#include "Property.h"
#include "WRenderer.h"
#include "WMesh.h"
#include "WMaterial.h"
#include "WMatrix4f.h"
#include "WVector4f.h"
#include "WVector3f.h"
#include "WVector2f.h"
#include "RStdVector.h"

using namespace Sentinel::Math;
using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	ref class RStdVector_Vertex;
	ref class RStdVector_Index;

	public ref class WMeshBuilder
	{
		DECLARE_REF( MeshBuilder );
	
	public:

		ref class WVertex
		{
			DECLARE_REF_EX( WVertex, MeshBuilder::Vertex );

		public:

			WVertex();
			WVertex( WVector3f^ pos );
			WVertex( WVector3f^ pos, WColorRGBA^ color );

			DECLARE_OP_DEREF( MeshBuilder::Vertex );

			DECLARE_PROPERTY( WVector3f^,	Position );

			WVector2f^		TextureCoords( TextureType type );

			DECLARE_PROPERTY( WVector3f^,	Normal );
			DECLARE_PROPERTY( int,			Color );
			DECLARE_PROPERTY( WVector4f^,	Tangent );

			float%			Weight( int index );
			int%			MatrixIndex( int index );

			DECLARE_PROPERTY( WMatrix4f^,	MatrixVertex );
		};

		DECLARE_CLASS_REF_BASE( MeshBuilder, Vertex );

		WMeshBuilder();
		
		//////////////////////////////////

		RStdVector_Vertex^		Vertex;
		RStdVector_Index^		Index;

		DECLARE_PROPERTY( WShader^,		Shader );
		DECLARE_PROPERTY( Sentinel::Systems::PrimitiveType, Primitive );
		DECLARE_PROPERTY( WVector4f^,	TextureScale );

		RTexture^		Texture( TextureType type );

		//////////////////////////////////

		DECLARE_OP_DEREF( MeshBuilder );

		void			ClearAll();
		void			ClearGeometry();

		// Index helper functions.
		//
		void			AddIndex( int i0 );										// Point
		void			AddIndex( int i0, int i1 );								// Line
		void			AddIndex( int i0, int i1, int i2 );						// Triangle
		void			AddIndex( int i0, int i1, int i2, int i3 );				// Quad
		void			AddIndex( int i0, int i1, int i2, int i3, int i4 );		// Polygon

		// Call this function only after all vertices have been added.
		// Used for normal mapping.
		//
		void			CalculateTangents( bool doNormals );

		// Create objects.
		// Based on geometry.h provided with OpenGL.
		//
		void			CreateQuad( float size, WVector3f^ normal );
		void			CreateCube( float size );
		void			CreateCylinder( float radius, float height, int slices, int stacks );
		void			CreateTetrahedron( float scale );
		void			CreateOctahedron( float radius );
		void			CreateDodecahedron( float scale );
		void			CreateWireSphere( float radius, int slices, int stacks );
		void			CreateSphere( float radius, int slices, int stacks );

		// Apply a matrix transform to a set a vertices.
		//
		void			ApplyMatrix( WMatrix4f^ mat );
		void			ApplyMatrix( WMatrix4f^ mat, int startVertex, int endVertex );

		// Returns the mesh created from the buffers.
		// Ensure mShader is set before calling this.
		//
		WMesh^			BuildMesh();
	};

	DECLARE_CLASS_REF( MeshBuilder );

	RStdVector_Class( Vertex, MeshBuilder::Vertex, WMeshBuilder::WVertex, WMeshBuilder::RVertex );
	RStdVector_Native( Index, UINT, int );
}}
