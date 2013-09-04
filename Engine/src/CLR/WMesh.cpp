#include "WMesh.h"

namespace Sentinel { namespace Assets
{
	DEFINE_REF_SHARED( Mesh );

	////////////////////////////////

	void WMesh::SetWorldTransform( WMatrix4f^ world )
	{
		mRef->SetWorldTransform( *world->GetRef() );
	}

	void WMesh::SetShadowTransform( WMatrix4f^ shadow )
	{
		mRef->SetShadowTransform( *shadow->GetRef() );
	}

	void WMesh::SetTextureScale( WVector4f^ scale )
	{
		mRef->SetTextureScale( *scale->GetRef() );
	}

	void WMesh::SetShader( WShader^ shader )
	{
		mRef->SetShader( shader->GetRef() );
	}

	void WMesh::SetMaterial( WMaterial^ material )
	{
		mRef->SetMaterial( *material->GetRef() );
	}

	void WMesh::SetTexture( WTexture^ texture, TextureType type )
	{
		mRef->SetTexture( texture->GetRef(), (Sentinel::TextureType)type );
	}

	WBuffer^ WMesh::GetVBO()
	{
		return gcnew RBuffer( mRef->GetVBO() );
	}

	WBuffer^ WMesh::GetIBO()
	{
		return gcnew RBuffer( mRef->GetIBO() );
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
