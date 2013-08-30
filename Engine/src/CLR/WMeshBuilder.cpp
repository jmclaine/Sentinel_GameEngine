#include "WMeshBuilder.h"
#include "Texture.h"

namespace Sentinel { namespace Assets
{
	WMeshBuilder::WMeshBuilder()
	{
		mRef = new MeshBuilder();
	}

	WMeshBuilder::~WMeshBuilder()
	{
		delete mRef;
	}

	WMeshBuilder::!WMeshBuilder()
	{
		delete mRef;
	}

	MeshBuilder* WMeshBuilder::GetRef()
	{
		return mRef;
	}

	//////////////////////////////////

	DEFINE_PROPERTY_P( MeshBuilder, Shader,		Shader );
	DEFINE_PROPERTY_E( MeshBuilder, Systems,	PrimitiveType, Primitive );
	DEFINE_PROPERTY_R( MeshBuilder, Material,	Material );
	DEFINE_PROPERTY_R( MeshBuilder, Vector4f, TextureScale );

	RTexture^ WMeshBuilder::Texture( TextureType type )
	{
		Sentinel::Texture*& tex = mRef->mTexture[ (int)type ];
		return (tex != NULL) ? gcnew RTexture( tex ) : nullptr;
	}
	
	//////////////////////////////////

	void WMeshBuilder::ClearAll()
	{
		mRef->ClearAll();
	}

	void WMeshBuilder::ClearGeometry()
	{
		mRef->ClearGeometry();
	}

	// Call this function only after all vertices have been added.
	// Used for normal mapping.
	//
	void WMeshBuilder::CalculateTangents( bool doNormals )
	{
		mRef->CalculateTangents( doNormals );
	}

	// Create objects.
	// Based on geometry.h provided with OpenGL.
	//
	void WMeshBuilder::CreateQuad( float size, WVector3f^ normal )
	{
		mRef->CreateQuad( size, *normal->GetRef() );
	}

	void WMeshBuilder::CreateCube( float size )
	{
		mRef->CreateCube( size );
	}

	void WMeshBuilder::CreateCylinder( float radius, float height, int slices, int stacks )
	{
		mRef->CreateCylinder( radius, height, slices, stacks );
	}

	void WMeshBuilder::CreateTetrahedron( float scale )
	{
		mRef->CreateTetrahedron( scale );
	}

	void WMeshBuilder::CreateOctahedron( float radius )
	{
		mRef->CreateOctahedron( radius );
	}

	void WMeshBuilder::CreateDodecahedron( float scale )
	{
		mRef->CreateDodecahedron( scale );
	}

	void WMeshBuilder::CreateWireSphere( float radius, int slices, int stacks )
	{
		mRef->CreateWireSphere( radius, slices, stacks );
	}

	void WMeshBuilder::CreateSphere( float radius, int slices, int stacks )
	{
		mRef->CreateSphere( radius, slices, stacks );
	}

	// Apply a matrix transform to a set a vertices.
	//
	void WMeshBuilder::ApplyMatrix( WMatrix4f^ mat )
	{
		mRef->ApplyMatrix( *mat->GetRef() );
	}

	void WMeshBuilder::ApplyMatrix( WMatrix4f^ mat, int startVertex, int endVertex )
	{
		mRef->ApplyMatrix( *mat->GetRef(), startVertex, endVertex );
	}

	// Returns the mesh created from the buffers.
	// Ensure mShader is set before calling this.
	//
	RMesh^ WMeshBuilder::BuildMesh()
	{
		return gcnew RMesh( mRef->BuildMesh() );
	}
}}
