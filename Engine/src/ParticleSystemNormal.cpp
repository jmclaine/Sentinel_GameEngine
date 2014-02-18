#include "ParticleSystem.h"
#include "ParticleEffect.h"
#include "Particle.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "MeshBuilder.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Texture.h"
#include "Sprite.h"
#include "RandomValue.h"
#include "Util.h"
#include "Point.h"
#include "ShaderManager.h"
#include "SpriteManager.h"
#include "MaterialManager.h"
#include "Archive.h"

namespace Sentinel
{
	class ParticleSystemNormal : public ParticleSystem
	{
		DECLARE_SERIAL_REGISTER();

		DEFINE_SERIAL_CLONE_INLINE( ParticleSystemNormal );

	private:

		std::shared_ptr< Material >	mMaterial;
		std::shared_ptr< Sprite >	mSprite;

		NormalParticle*				mParticle;

	public:

		ParticleSystemNormal()
		{}

		ParticleSystemNormal( Renderer* renderer, GameWorld* world ) :
			ParticleSystem( renderer, world )
		{}

		ParticleSystemNormal( Renderer* renderer, GameWorld* world, std::shared_ptr< Material > material, std::shared_ptr< Sprite > sprite, UINT maxParticles ) :
			ParticleSystem( renderer, world )
		{
			Build( material, sprite, maxParticles );
		};

		~ParticleSystemNormal()
		{
			delete[] mParticle;
		}

		void Build( Renderer* renderer, GameWorld* world, std::shared_ptr< Material > material, std::shared_ptr< Sprite > sprite, UINT maxParticles )
		{
			_ASSERT( renderer );
			_ASSERT( world );

			mRenderer  = renderer;
			mGameWorld = world;

			Build( material, sprite, maxParticles );
		}

		void Build( std::shared_ptr< Material > material, std::shared_ptr< Sprite > sprite, UINT maxParticles )
		{
			mMaterial		= material;
			mSprite			= sprite;
			mMaxParticles	= maxParticles;

			_ASSERT( mMaterial.get() );
			_ASSERT( mSprite.get() );
			_ASSERT( mMaxParticles > 0 );
			
			mParticle = new NormalParticle[ maxParticles ];

			MeshBuilder builder;

			builder.mLayout = material->mShader->Layout();

			for( UINT x = 0; x < maxParticles; ++x )
			{
				builder.mVertex.push_back( MeshBuilder::Vertex( Vector3f( 0.0f, 0.0f, 0.0f )));
				builder.mIndex.push_back( x );
			}

			builder.mPrimitive = POINT_LIST;

			mMesh = std::shared_ptr< Mesh >(builder.BuildMesh( mRenderer ));

			if( !mMesh )
				throw AppException( "Failed to create Mesh in ParticleSystemNormal::Startup" );

			mMesh->mMaterial = material;

			mMesh->mMatrixWorld.Identity();
		}

		void Update( float DT )
		{
			CameraComponent* camera = mGameWorld->GetCamera();
			const TransformComponent* cameraTrans = camera->GetTransform();

			ParticleSystem::Update( DT );

			BYTE* verts = (BYTE*)mMesh->mVBO->Lock();

			for( UINT x = 0; x < (UINT)mNumParticles; ++x )
			{
				NormalParticle& particle = mParticle[ x ];

				*(Quad*)verts = mSprite->GetFrame( particle.mFrame );
				verts += sizeof( Quad );

				*(UINT*)verts = particle.mColor.ToUINT();
				verts += sizeof( UINT );

				static Matrix4f matrixParticle;
				matrixParticle.World( particle.mPosition, Quatf( particle.mRotation ), particle.mScale );
				
				static Matrix4f matrixBillboard;
				matrixBillboard.BillboardAxis( mMesh->mMatrixWorld.Transform( particle.mPosition ), cameraTrans->mPosition, Vector3f( 0, 1, 0 ));

				*(Matrix4f*)verts = camera->GetMatrixFinal() * mMesh->mMatrixWorld * matrixParticle * matrixBillboard;
				verts += sizeof( Matrix4f );
			}

			mMesh->mVBO->Unlock();

			mMesh->Draw( mRenderer, mGameWorld, mNumParticles );
		}

		Particle& GetParticle( UINT index )
		{
			_ASSERT( index < mMaxParticles );

			return mParticle[ index ];
		}

		void Save( Archive& archive )
		{
			SERIAL_REGISTER.Save( archive );
			
			ParticleSystem::Save( archive );

			archive.Write( &mGameWorld->mMaterialManager->Get( mMaterial ));
			archive.Write( &mGameWorld->mSpriteManager->Get( mSprite ));
		}

		void Load( Archive& archive )
		{
			ParticleSystem::Load( archive );

			std::string material;
			archive.Read( &material );

			std::string sprite;
			archive.Read( &sprite );

			Build( mGameWorld->mMaterialManager->Get( material ), mGameWorld->mSpriteManager->Get( sprite ), mMaxParticles );
		}
	};

	DEFINE_SERIAL_REGISTER( ParticleSystemNormal );

	ParticleSystem* BuildParticleSystemNormal( Renderer* renderer, GameWorld* world, 
											   std::shared_ptr< Material > material, 
											   std::shared_ptr< Sprite > sprite, 
											   UINT maxParticles )
	{
		return new ParticleSystemNormal( renderer, world, material, sprite, maxParticles );
	}

	ParticleSystem* BuildParticleSystemNormal( Renderer* renderer, GameWorld* world )
	{
		return new ParticleSystemNormal( renderer, world );
	}
}
