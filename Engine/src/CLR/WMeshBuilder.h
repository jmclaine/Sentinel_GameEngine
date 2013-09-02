#pragma once
/*
Removed creation of custom objects due to memory
consumption of multiple copies of the Vertex
and Index buffers.
*/
#include "MeshBuilder.h"
#include "Property.h"
#include "WRenderer.h"
#include "WMesh.h"
#include "WMaterial.h"
#include "WMatrix4f.h"
#include "WVector3f.h"
#include "WVector4f.h"

using namespace Sentinel::Math;

namespace Sentinel { namespace Assets
{
	public ref class WMeshBuilder
	{
	private:

		MeshBuilder*	mRef;

	public:

		WMeshBuilder();
		~WMeshBuilder();
		!WMeshBuilder();

		MeshBuilder*	GetRef();

		//////////////////////////////////

		DECLARE_PROPERTY( WShader^,		Shader );
		DECLARE_PROPERTY( Sentinel::Systems::PrimitiveType, Primitive );
		DECLARE_PROPERTY( WVector4f^,	TextureScale );

		RTexture^		Texture( TextureType type );

		//////////////////////////////////

		void			ClearAll();
		void			ClearGeometry();

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
}}
