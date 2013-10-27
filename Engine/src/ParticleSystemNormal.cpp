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

namespace Sentinel
{
	class ParticleSystemNormal : public ParticleSystem
	{
	private:

		std::shared_ptr< Sprite >		mSprite;

	public:

		ParticleSystemNormal( Renderer* renderer, GameWorld* world, std::shared_ptr< Sprite > sprite, UINT maxParticles ) :
			ParticleSystem( renderer, world, maxParticles ),
			mSprite( sprite )
		{
			_ASSERT( mSprite );
			_ASSERT( mSprite->GetTexture() );
			_ASSERT( mSprite->GetShader() );
			
			for( UINT x = 0; x < maxParticles; ++x )
				mParticle[ x ] = new NormalParticle();

			MeshBuilder builder;

			builder.mShader = mSprite->GetShader();

			for( UINT x = 0; x < maxParticles; ++x )
			{
				builder.mVertex.push_back( MeshBuilder::Vertex( Vector3f( 0.0f, 0.0f, 0.0f )));
				builder.mIndex.push_back( x );
			}

			builder.mPrimitive = POINT_LIST;

			mMesh = builder.BuildMesh( mRenderer );

			if( !mMesh )
				throw AppException( "Failed to create Mesh in ParticleSystemNormal::Startup" );

			mMesh->mTexture[ TEXTURE_DIFFUSE ] = mSprite->GetTexture();

			mMesh->mTextureScale = Vector4f( 1.0f / (float)mSprite->GetDimension().x,
											 1.0f / (float)mSprite->GetDimension().y, 0, 0 );
		};

		void Update( float DT )
		{
			ParticleSystem::Update( DT );

			// Prepare to render particles.
			//
			BYTE* verts = (BYTE*)mMesh->mVBO->Lock();

			for( UINT x = 0; x < (UINT)mNumParticles; ++x )
			{
				NormalParticle* particle = static_cast< NormalParticle* >(mParticle[ x ]);

				// Prepare particle in buffer.
				//
				*(Vector2f*)verts = mSprite->GetFrameCoords( particle->mFrame );
				verts += sizeof( Vector2f );

				*(UINT*)verts = particle->mColor.ToUINT();
				verts += sizeof( UINT );

				MATRIX_TRANSLATION.Translate( particle->mPosition );
				MATRIX_ROTATION.Rotate( particle->mRotation );
				MATRIX_SCALE.Scale( particle->mScale );

				mMesh->mMatrixWorld.BillboardWorld( particle->mPosition, mWorld->GetCamera()->GetTransform()->mPosition, Vector3f( 0, 1, 0 ));
				*(Matrix4f*)verts = mMesh->mMatrixWorld * MATRIX_TRANSLATION * MATRIX_ROTATION * MATRIX_SCALE;
				verts += sizeof( Matrix4f );
			}

			mMesh->mVBO->Unlock();

			// Render particles...for now.
			//
			mRenderer->SetCull( CULL_NONE );
			mRenderer->SetBlend( BLEND_PARTICLE );
			mRenderer->SetDepthStencilState( STENCIL_PARTICLE );

			mMesh->mMatrixWorld.Identity();
			mMesh->Draw( mRenderer, mWorld, mNumParticles );

			mRenderer->SetCull( CULL_CCW );
			mRenderer->SetBlend( BLEND_ALPHA );
			mRenderer->SetDepthStencilState( STENCIL_DEFAULT );
		}
	};

	ParticleSystem* BuildParticleSystemNormal( Renderer* renderer, GameWorld* world, std::shared_ptr< Sprite > sprite, UINT maxParticles )
	{
		return new ParticleSystemNormal( renderer, world, sprite, maxParticles );
	}
}