#include "WMesh.h"
#include "WRenderer.h"
#include "WGameWorld.h"
#include "WBuffer.h"
#include "WShader.h"
#include "WTexture.h"
#include "WMaterial.h"
#include "WMatrix4f.h"
#include "WVector4f.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_SHARED( Mesh );

	DEFINE_OP_SHARED( Mesh );

	////////////////////////////////

	bool WMesh::operator == ( WMesh^ mesh0, WMesh^ mesh1 )
	{
		return mesh0->mRef == mesh1->mRef;
	}

	DEFINE_PROPERTY_E( Mesh, PrimitiveType, Primitive );
	
	DEFINE_PROPERTY_RP( Mesh, Buffer,		VBO );
	DEFINE_PROPERTY_RP( Mesh, Buffer,		IBO );

	DEFINE_PROPERTY_R(  Mesh, Matrix4f,		MatrixWorld );
	DEFINE_PROPERTY_R(  Mesh, Matrix4f,		MatrixShadow );
	DEFINE_PROPERTY_R(  Mesh, Vector4f,		TextureScale );
		
	DEFINE_PROPERTY_RT( Mesh, Shader,		Shader );
	DEFINE_PROPERTY_R(  Mesh, Material,		Material );

	RTexture^ WMesh::Texture( TextureType type )
	{
		return gcnew RTexture( mRef->mTexture[ (int)type ] );
	}

	void WMesh::Draw( WRenderer^ renderer, WGameWorld^ world )
	{
		mRef->Draw( renderer, world );
	}

	void WMesh::Draw( WRenderer^ renderer, WGameWorld^ world, UINT count )
	{
		mRef->Draw( renderer, world, count );
	}

	////////////////////////////////

	DEFINE_CLASS_REF_SHARED( Mesh );
}}
