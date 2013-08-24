#include "RMesh.h"

namespace Sentinel { namespace Assets
{
	RMesh::RMesh( Mesh* mesh )
	{
		mRef = mesh;
	}

	RMesh::RMesh( std::shared_ptr< Mesh > mesh )
	{
		mRef = mesh.get();
	}

	RMesh::~RMesh()
	{}

	std::shared_ptr< Mesh > RMesh::GetRef()
	{
		return mRef.get();
	}

	////////////////////////////////

	void RMesh::SetWorldTransform( WMatrix4f^ world )
	{
		mRef->SetWorldTransform( *world->GetRef() );
	}

	void RMesh::SetShadowTransform( WMatrix4f^ shadow )
	{
		mRef->SetShadowTransform( *shadow->GetRef() );
	}

	void RMesh::SetTextureScale( WVector4f^ scale )
	{
		mRef->SetTextureScale( *scale->GetRef() );
	}

	void RMesh::SetShader( WShader^ shader )
	{
		mRef->SetShader( shader->GetRef() );
	}

	void RMesh::SetMaterial( WMaterial^ material )
	{
		mRef->SetMaterial( *material->GetRef() );
	}

	void RMesh::SetTexture( RTexture^ texture, TextureType type )
	{
		mRef->SetTexture( texture->GetRef(), (Sentinel::TextureType)type );
	}

	RBuffer^ RMesh::GetVBO()
	{
		return gcnew RBuffer( mRef->GetVBO() );
	}

	RBuffer^ RMesh::GetIBO()
	{
		return gcnew RBuffer( mRef->GetIBO() );
	}

	void RMesh::Draw()
	{
		mRef->Draw();
	}

	void RMesh::Draw( UINT count )
	{
		mRef->Draw( count );
	}
}}
