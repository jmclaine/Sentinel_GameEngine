#include "SpriteSystem.h"
#include "MeshBuilder.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Sprite.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "CameraComponent.h"
#include "TransformComponent.h"

namespace Sentinel
{
	SpriteSystem::SpriteSystem( Renderer* renderer, std::shared_ptr< Shader > shader, UINT maxSprites ) :
		mRenderer( renderer ),
		mShader( shader ),
		mMaxSprites( maxSprites ),
		mNumSprites( 0 )
	{
		_ASSERT( renderer );
		_ASSERT( shader );
		_ASSERT( maxSprites > 0 );

		mStorage = new Storage[ maxSprites ];

		MeshBuilder builder;

		builder.mShader = mShader;

		for( UINT x = 0; x < maxSprites; ++x )
		{
			builder.mVertex.push_back( MeshBuilder::Vertex( Vector3f( 0.0f, 0.0f, 0.0f )));
			builder.mIndex.push_back( x );
		}

		builder.mPrimitive = POINT_LIST;

		mMesh = builder.BuildMesh( mRenderer );

		if( !mMesh )
			throw AppException( "Failed to create Mesh in SpriteSystem" );
	}

	SpriteSystem::~SpriteSystem()
	{
		delete[] mStorage;

		delete mMesh;
	}

	Renderer* SpriteSystem::GetRenderer()
	{
		return mRenderer;
	}

	std::shared_ptr< Shader > SpriteSystem::GetShader()
	{
		return mShader;
	}

	void SpriteSystem::Clear()
	{
		mNumSprites = 0;
	}

	void SpriteSystem::Draw( UINT frame, const ColorRGBA& color, const Matrix4f& matWorld )
	{
		_ASSERT( mNumSprites < mMaxSprites );

		Storage& store = mStorage[ mNumSprites ];

		store.mFrame		= frame;
		store.mColor		= color;
		store.mMatrixWorld	= matWorld;
		
		++mNumSprites;
	}

	void SpriteSystem::Present()
	{
		BYTE* verts = (BYTE*)mMesh->mVBO->Lock();

		for( UINT x = 0; x < (UINT)mNumSprites; ++x )
		{
			Storage& store = mStorage[ x ];

			*(Quad*)verts = mSprite->GetFrame( store.mFrame );
			verts += sizeof( Quad );

			*(UINT*)verts = store.mColor.ToUINT();
			verts += sizeof( UINT );

			*(Matrix4f*)verts = store.mMatrixWorld;
			verts += sizeof( Matrix4f );
		}

		mMesh->mVBO->Unlock();

		mMesh->mShader = mShader;
		mMesh->mTexture[ TEXTURE_DIFFUSE ] = mSprite->mTexture;

		mMesh->Draw( mRenderer, NULL, mNumSprites );
	}
}