#include "ParticleSystem.h"
#include "MeshBuilder.h"
#include "SpriteComponent.h"

namespace Sentinel
{
	ParticleSystem::ParticleSystem() :
		mMesh( NULL ), mCount( 0 ), mVertex( NULL )
	{}

	ParticleSystem::~ParticleSystem()
	{
		Shutdown();
	}
		
	// Based on a geometry shader that generates quads from points.
	//
	void ParticleSystem::Startup( Shader* shader, UINT maxSprites )
	{
		MeshBuilder builder;

		builder.mShader = shader;

		for( UINT x = 0; x < maxSprites; ++x )
		{
			builder.mVertex.push_back( MeshBuilder::Vertex( Vector3f( 0.0f, 0.0f, 0.0f )));
			builder.mIndex.push_back( x );
		}

		builder.mPrimitive = POINT_LIST;

		mMesh = builder.BuildMesh();
	}

	void ParticleSystem::Shutdown()
	{
		if( mMesh )
		{
			delete mMesh;
			mMesh = NULL;
		}
	}

	// Prepare for batched Sprite rendering.
	//
	void ParticleSystem::Begin( SpriteComponent* sprite )
	{
		_ASSERT( sprite != NULL );
		_ASSERT( sprite->mTexture != NULL );

		mCount = 0;

		mMesh->mTexture[ TEXTURE_DIFFUSE ] = sprite->mTexture;

		mMesh->mTextureScale = Vector4f( (float)sprite->mSpriteSize.x / (float)sprite->mTexture->Width(),
									     (float)sprite->mSpriteSize.y / (float)sprite->mTexture->Height(), 0, 0 );

		mVertex = mMesh->mVBO->Lock();
	}

	// Render Sprites within the batch.
	//
	void ParticleSystem::End()
	{
		mMesh->mVBO->Unlock();

		mMesh->Draw( mCount );
	}
}
