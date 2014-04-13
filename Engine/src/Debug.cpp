#include "Debug.h"
#include "MeshBuilder.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Shader.h"
#include "VertexLayout.h"
#include "Material.h"

namespace Sentinel
{
	Debug::Debug( Renderer* renderer, GameWorld* world, std::shared_ptr< Material > material, UINT maxLines ) :
		mRenderer( renderer ),
		mWorld( world ),
		mMaterial( material ),
		mMaxLines( maxLines )
	{
		_ASSERT( renderer );
		_ASSERT( material.get() );
		_ASSERT( material->mShader.get() );
		_ASSERT( maxLines > 0 );

		MeshBuilder builder;

		builder.mLayout = material->mShader->Layout();

		for( UINT x = 0; x < maxLines; ++x )
		{
			UINT index = x << 1;

			builder.mVertex.push_back( MeshBuilder::Vertex( Vector3f( 0.0f, 0.0f, 0.0f )));
			builder.mVertex.push_back( MeshBuilder::Vertex( Vector3f( 0.0f, 0.0f, 0.0f )));
		}

		builder.mPrimitive = PrimitiveFormat::LINES;

		mMesh = builder.BuildMesh( mRenderer, false );

		if( !mMesh )
			throw AppException( "Failed to create Mesh in Debug" );
	}

	Debug::~Debug()
	{
		delete mMesh;
	}

	Renderer* Debug::GetRenderer()
	{
		return mRenderer;
	}

	GameWorld* Debug::GetWorld()
	{
		return mWorld;
	}

	/////////////////////////////////////

	void Debug::Clear()
	{
		mNumLines = 0;
	}

	void Debug::DrawLine( const Vector3f& start, const Vector3f& end )
	{
		Vector3f* data = (Vector3f*)mMesh->mVertexBuffer->Lock() + (mNumLines << 1);

		*data = start;
		*(data+1) = end;

		mMesh->mVertexBuffer->Unlock();

		++mNumLines;
	}

	void Debug::Present()
	{
		mMesh->mMaterial = mMaterial;

		mMesh->Draw( mRenderer, mWorld, mNumLines << 1 );
	}
}
