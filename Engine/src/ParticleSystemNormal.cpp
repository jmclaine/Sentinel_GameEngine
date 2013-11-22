#include "ParticleSystem.h"
#include "ParticleEffect.h"
#include "Particle.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "MeshBuilder.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "Mesh.h"
#include "Shader.h"
#include "Buffer.h"
#include "Texture.h"
#include "Sprite.h"
#include "RandomValue.h"
#include "Util.h"
#include "Point.h"
#include "SpriteManager.h"
#include "Archive.h"

namespace Sentinel
{
	class ParticleSystemNormal : public ParticleSystem
	{
		DECLARE_SERIAL_REGISTER();

	public:

		DEFINE_SERIAL_CLONE_INLINE( ParticleSystemNormal );

	private:

		std::shared_ptr< Sprite >	mSprite;

		NormalParticle*				mParticle;

	public:

		ParticleSystemNormal()
		{}

		ParticleSystemNormal( Renderer* renderer, GameWorld* world ) :
			ParticleSystem( renderer, world )
		{}

		ParticleSystemNormal( Renderer* renderer, GameWorld* world, std::shared_ptr< Sprite > sprite, UINT maxParticles ) :
			ParticleSystem( renderer, world )
		{
			Build( sprite, maxParticles );
		};

		~ParticleSystemNormal()
		{
			delete[] mParticle;
		}

		void Build( Renderer* renderer, GameWorld* world, std::shared_ptr< Sprite > sprite, UINT maxParticles )
		{
			_ASSERT( renderer );
			_ASSERT( world );

			mRenderer  = renderer;
			mGameWorld = world;

			Build( sprite, maxParticles );
		}

		void Build( std::shared_ptr< Sprite > sprite, UINT maxParticles )
		{
			mSprite			= sprite;
			mMaxParticles	= maxParticles;

			_ASSERT( mSprite );
			_ASSERT( mSprite->mTexture );
			_ASSERT( mSprite->mShader );
			
			mParticle = new NormalParticle[ maxParticles ];

			MeshBuilder builder;

			builder.mShader = mSprite->mShader;

			for( UINT x = 0; x < maxParticles; ++x )
			{
				builder.mVertex.push_back( MeshBuilder::Vertex( Vector3f( 0.0f, 0.0f, 0.0f )));
				builder.mIndex.push_back( x );
			}

			builder.mPrimitive = POINT_LIST;

			mMesh = std::shared_ptr< Mesh >(builder.BuildMesh( mRenderer ));

			if( !mMesh )
				throw AppException( "Failed to create Mesh in ParticleSystemNormal::Startup" );

			mMesh->mTexture[ TEXTURE_DIFFUSE ] = mSprite->mTexture;

			mMesh->mTextureScale = Vector4f( 1.0f / (float)mSprite->mTexture->Width(),
											 1.0f / (float)mSprite->mTexture->Height(), 0, 0 );
		}

		void Update( float DT )
		{
			ParticleSystem::Update( DT );

			// Prepare to render particles.
			//
			BYTE* verts = (BYTE*)mMesh->mVBO->Lock();

			for( UINT x = 0; x < (UINT)mNumParticles; ++x )
			{
				NormalParticle& particle = mParticle[ x ];

				// Prepare particle in buffer.
				//
				*(Quad*)verts = mSprite->GetFrame( particle.mFrame );
				verts += sizeof( Quad );

				*(UINT*)verts = particle.mColor.ToUINT();
				verts += sizeof( UINT );

				static Matrix4f matrixParticle;
				matrixParticle.World( particle.mPosition, Quatf( particle.mRotation ), particle.mScale );
				
				mMesh->mMatrixWorld.BillboardWorld( particle.mPosition, mGameWorld->GetCamera()->GetTransform()->mPosition, Vector3f( 0, 1, 0 ));
				*(Matrix4f*)verts = mGameWorld->GetCamera()->GetMatrixFinal() * mMesh->mMatrixWorld * matrixParticle;
				verts += sizeof( Matrix4f );
			}

			mMesh->mVBO->Unlock();

			// Render particles.
			//
			mRenderer->SetCull( CULL_NONE );
			mRenderer->SetBlend( BLEND_PARTICLE );
			mRenderer->SetDepthStencilState( STENCIL_PARTICLE );

			mMesh->mMatrixWorld.Identity();
			mMesh->Draw( mRenderer, mGameWorld, mNumParticles );

			mRenderer->SetCull( CULL_CCW );
			mRenderer->SetBlend( BLEND_ALPHA );
			mRenderer->SetDepthStencilState( STENCIL_DEFAULT );
		}

		Particle& GetParticle( UINT index )
		{
			_ASSERT( index < mMaxParticles );

			return mParticle[ index ];
		}

		void Save( Archive& archive )
		{
			mSerialRegistry.Save( archive );
			
			ParticleSystem::Save( archive );

			archive.Write( &mGameWorld->mSpriteManager->Get( mSprite ));
		}

		void Load( Archive& archive )
		{
			ParticleSystem::Load( archive );

			std::string sprite;
			archive.Read( &sprite );

			Build( mGameWorld->mSpriteManager->Get( sprite ), mMaxParticles );
		}
	};

	DEFINE_SERIAL_REGISTER( ParticleSystemNormal );

	ParticleSystem* BuildParticleSystemNormal( Renderer* renderer, GameWorld* world, std::shared_ptr< Sprite > sprite, UINT maxParticles )
	{
		return new ParticleSystemNormal( renderer, world, sprite, maxParticles );
	}

	ParticleSystem* BuildParticleSystemNormal( Renderer* renderer, GameWorld* world )
	{
		return new ParticleSystemNormal( renderer, world );
	}
}
