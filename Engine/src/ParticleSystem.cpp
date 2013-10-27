#include "ParticleSystem.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "MeshBuilder.h"
#include "Mesh.h"
#include "Shader.h"
#include "Buffer.h"
#include "Texture.h"
#include "Sprite.h"
#include "RandomValue.h"
#include "Util.h"

namespace Sentinel
{
	Matrix4f ParticleSystem::MATRIX_TRANSLATION;
	Matrix4f ParticleSystem::MATRIX_SCALE;
	Matrix4f ParticleSystem::MATRIX_ROTATION;

	ParticleSystem::ParticleSystem( Renderer* renderer, GameWorld* world, UINT maxParticles ) :
		mRenderer( renderer ),
		mWorld( world ),
		mNumParticles( 0 ),
		mMaxParticles( maxParticles ),
		mMesh( NULL ),
		mSpawnTime( 0 ),
		mSpawnRate( 0.01f ),
		mIsActive( false )
	{
		_ASSERT( renderer );
		_ASSERT( world );
		_ASSERT( maxParticles > 0 );

		mParticle = new Particle*[ maxParticles ];
	}

	ParticleSystem::~ParticleSystem()
	{
		for( UINT x = 0; x < (UINT)mEffect.size(); ++x )
			delete mEffect[ x ];

		for( UINT x = 0; x < mMaxParticles; ++x )
			delete mParticle[ x ];

		delete[] mParticle;

		delete mMesh;

		Shutdown();
	}

	void ParticleSystem::Startup()
	{
		mIsActive = true;
	}
		
	void ParticleSystem::Update( float DT )
	{
		// Generate particles.
		//
		if( mNumParticles < mMaxParticles )
		{
			mSpawnTime += DT;

			// Spawn as many particles as spawner will allow.
			//
			while( mSpawnTime >= mSpawnRate && mNumParticles < mMaxParticles )
			{
				mSpawnTime -= mSpawnRate;

				Particle* particle = mParticle[ mNumParticles ];

				particle->mElapsedTime = 0;
				particle->mLifetime    = RandomValue( mMinLifetime, mMaxLifetime );
				particle->mEffectIndex = 0;

				++mNumParticles;
			}

			// Prevent SpawnTime overrun.
			//
			while( mSpawnTime >= mSpawnRate )
				mSpawnTime -= mSpawnRate;
		}

		for( int x = 0; x < (int)mNumParticles; ++x )
		{
			Particle* particle = mParticle[ x ];

			// Startup effects in time order.
			//
			for( UINT y = particle->mEffectIndex; y < (UINT)mEffect.size(); ++y )
			{
				if( particle->mElapsedTime >= mEffect[ y ]->mStartTime )
				{
					mEffect[ y ]->Startup( particle );

					++particle->mEffectIndex;
				}
			}

			// Update particles with effects.
			//
			for( UINT y = 0; y < particle->mEffectIndex; ++y )
				mEffect[ y ]->Update( particle );

			particle->Update( DT );

			// Replace dead particles.
			//
			if( particle->mElapsedTime >= particle->mLifetime )
			{
				--mNumParticles;

				*mParticle[ x ] = *mParticle[ mNumParticles ];

				--x;
			}
		}
	}

	void ParticleSystem::Shutdown()
	{
		mNumParticles = 0;

		mIsActive = false;
	}
}
