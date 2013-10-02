#include "WMeshBuilder.h"
#include "Texture.h"

namespace Sentinel { namespace Assets
{
	DEFINE_REF_EX_BASE( WMeshBuilder, WVertex, MeshBuilder::Vertex );

	WMeshBuilder::WVertex::WVertex()
	{
		mRef = new MeshBuilder::Vertex();
	}

	WMeshBuilder::WVertex::WVertex( WVector3f^ pos )
	{
		mRef = new MeshBuilder::Vertex( pos );
	}

	WMeshBuilder::WVertex::WVertex( WVector3f^ pos, WColorRGBA^ color )
	{
		mRef = new MeshBuilder::Vertex( pos, color );
	}

	DEFINE_OP_DEREF_EX( WMeshBuilder::WVertex, MeshBuilder::Vertex );

	DEFINE_PROPERTY_RSB( MeshBuilder, Vertex, Vector3f, Position );

	WVector2f^ WMeshBuilder::WVertex::TextureCoords( TextureType type )
	{
		return gcnew RVector2f( &mRef->mTextureCoords[ (int)type ] );
	}

	DEFINE_PROPERTY_RSB( MeshBuilder, Vertex, Vector3f, Normal );
	DEFINE_PROPERTY_MB(  MeshBuilder, Vertex, int,	    Color );
	DEFINE_PROPERTY_RSB( MeshBuilder, Vertex, Vector4f, Tangent );

	float% WMeshBuilder::WVertex::Weight( int index )
	{
		return mRef->mWeight[ index ];
	}

	int% WMeshBuilder::WVertex::MatrixIndex( int index )
	{
		return mRef->mMatrixIndex[ index ];
	}

	DEFINE_PROPERTY_RSB( MeshBuilder, Vertex, Matrix4f, MatrixVertex );

	//////////////////////////////////

	DEFINE_CLASS_REF_BASE( MeshBuilder, Vertex );

	//////////////////////////////////

	DEFINE_REF( MeshBuilder );

	WMeshBuilder::WMeshBuilder()
	{
		mRef  = new MeshBuilder();

		Vertex = gcnew RStdVector_Vertex( &mRef->mVertex );
		Index  = gcnew RStdVector_Index( &mRef->mIndex );
	}

	//////////////////////////////////

	DEFINE_PROPERTY_RT( MeshBuilder, Shader,	Shader );
	DEFINE_PROPERTY_E(  MeshBuilder, Systems,	PrimitiveType, Primitive );
	DEFINE_PROPERTY_R(  MeshBuilder, Vector4f,	TextureScale );

	RTexture^ WMeshBuilder::Texture( TextureType type )
	{
		return gcnew RTexture( mRef->mTexture[ (int)type ] );
	}
	
	//////////////////////////////////

	DEFINE_OP_DEREF( MeshBuilder );

	void WMeshBuilder::ClearAll()
	{
		mRef->ClearAll();
	}

	void WMeshBuilder::ClearGeometry()
	{
		mRef->ClearGeometry();
	}

	// Index helper functions.
	//
	void WMeshBuilder::AddIndex( int i0 )
	{
		mRef->AddIndex( (UINT)i0 );
	}

	void WMeshBuilder::AddIndex( int i0, int i1 )
	{
		mRef->AddIndex( (UINT)i0, (UINT)i1 );
	}

	void WMeshBuilder::AddIndex( int i0, int i1, int i2 )
	{
		mRef->AddIndex( (UINT)i0, (UINT)i1, (UINT)i2 );
	}

	void WMeshBuilder::AddIndex( int i0, int i1, int i2, int i3 )
	{
		mRef->AddIndex( (UINT)i0, (UINT)i1, (UINT)i2, (UINT)i3 );
	}

	void WMeshBuilder::AddIndex( int i0, int i1, int i2, int i3, int i4 )
	{
		mRef->AddIndex( (UINT)i0, (UINT)i1, (UINT)i2, (UINT)i3, (UINT)i4 );
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
	WMesh^ WMeshBuilder::BuildMesh()
	{
		return gcnew WMesh( std::shared_ptr< Mesh >( mRef->BuildMesh() ));
	}

	//////////////////////////////////

	DEFINE_CLASS_REF( MeshBuilder );
}}
