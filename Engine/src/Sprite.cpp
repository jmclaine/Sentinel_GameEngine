#include "Sprite.h"

namespace Sentinel
{
	Sprite::Sprite() :
		mMesh( NULL ), mCount( 0 ), mVertex( NULL ), mCamera( NULL ), mPrevCamera( NULL )
	{}
		
	// Based on a geometry shader that generates quads from points.
	//
	void Sprite::Startup( Shader* shader, UINT maxSprites )
	{
		MeshBuilder builder;

		builder.mShader = shader;

		for( UINT x = 0; x < maxSprites; ++x )
		{
			builder.mVertex.push_back( MeshBuilder::Vertex( vec3f( 0.0f, 0.0f, 0.0f )));
			builder.mIndex.push_back( x );
		}

		builder.mPrimitive = POINT_LIST;
		mMesh = builder.BuildMesh();
	}

	void Sprite::Shutdown()
	{
		if( mMesh )
		{
			delete mMesh;
			mMesh = NULL;
		}
	}

	// Prepare for batched Sprite rendering.
	//
	void Sprite::Begin( SpriteComponent* sprite, OrthographicCameraComponent* orthoCamera )
	{
		_ASSERT( sprite != NULL );
		_ASSERT( sprite->mTexture != NULL );

		mCount = 0;
		mPrevCamera = GameWorld::Inst()->mCamera;

		mCamera = (!orthoCamera) ? GameWorld::Inst()->mCamera : orthoCamera;
			
		mMesh->mMaterial = sprite->mMaterial;
		mMesh->mTexture[ TEXTURE_DIFFUSE ] = sprite->mTexture;
		mMesh->mTextureScale = vec4f( (float)sprite->mSpriteSize.x / (float)sprite->mTexture->mWidth,
										(float)sprite->mSpriteSize.y / (float)sprite->mTexture->mHeight, 0, 0 );

		mVertex = mMesh->mVBO->Lock();

		GameWorld::Inst()->mCamera = mCamera;
	}

	// Render Sprites within the batch.
	//
	void Sprite::End()
	{
		mMesh->mVBO->Unlock();
		mMesh->Draw( mCount );

		GameWorld::Inst()->mCamera = mPrevCamera;
	}
}
