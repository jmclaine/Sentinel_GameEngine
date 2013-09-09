#include "WMesh.h"

namespace Sentinel { namespace Assets
{
	DEFINE_REF_SHARED( Mesh );

	////////////////////////////////

	DEFINE_PROPERTY_E( Mesh, Systems, PrimitiveType, Primitive );
	
	DEFINE_PROPERTY_PR( Mesh, Buffer,		VBO );
	DEFINE_PROPERTY_PR( Mesh, Buffer,		IBO );

	DEFINE_PROPERTY_R(  Mesh, Matrix4f,		MatrixWorld );
	DEFINE_PROPERTY_R(  Mesh, Matrix4f,		MatrixShadow );
	DEFINE_PROPERTY_R(  Mesh, Vector4f,		TextureScale );
		
	DEFINE_PROPERTY_PR( Mesh, Shader,		Shader );
	DEFINE_PROPERTY_R(  Mesh, Material,		Material );

	RTexture^ WMesh::Texture( TextureType type )
	{
		return gcnew RTexture( mRef->mTexture[ (int)type ] );
	}

	void WMesh::Draw()
	{
		mRef->Draw();
	}

	void WMesh::Draw( UINT count )
	{
		mRef->Draw( count );
	}
}}
