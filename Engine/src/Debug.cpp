#include "Debug.h"
#include "MeshBuilder.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Shader.h"

namespace Sentinel
{
	Debug::Debug( Renderer* renderer, GameWorld* world, std::shared_ptr< Shader > shader, UINT maxLines ) :
		mRenderer( renderer ),
		mWorld( world ),
		mShader( shader ),
		mMaxLines( maxLines )
	{
		_ASSERT( renderer );
		_ASSERT( shader );
		_ASSERT( maxLines > 0 );

		MeshBuilder builder;

		builder.mShader = mShader;

		for( UINT x = 0; x < maxLines; ++x )
		{
			UINT index = x << 1;

			builder.mVertex.push_back( MeshBuilder::Vertex( Vector3f( 0.0f, 0.0f, 0.0f )));
			builder.mVertex.push_back( MeshBuilder::Vertex( Vector3f( 0.0f, 0.0f, 0.0f )));

			builder.mIndex.push_back( index );
			builder.mIndex.push_back( index+1 );
		}

		builder.mPrimitive = LINE_LIST;

		mMesh = builder.BuildMesh( mRenderer );

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

	std::shared_ptr< Shader > Debug::GetShader()
	{
		return mShader;
	}

	/////////////////////////////////////

	void Debug::Clear()
	{
		mNumLines = 0;
	}

	void Debug::DrawLine( const Vector3f& start, const Vector3f& end )
	{
		Vector3f* data = (Vector3f*)mMesh->mVBO->Lock() + (mNumLines << 1);

		*data = start;
		*(data+1) = end;

		mMesh->mVBO->Unlock();

		++mNumLines;
	}

	void Debug::Present()
	{
		mMesh->Draw( mRenderer, mWorld, mNumLines << 1 );
	}
}
