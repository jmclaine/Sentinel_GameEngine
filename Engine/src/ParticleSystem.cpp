#include "ParticleSystem.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Component/Camera.h"
#include "Archive.h"
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
	ParticleSystem::ParticleSystem()
	{}

	ParticleSystem::ParticleSystem( Renderer* renderer, GameWorld* world ) :
		mNumParticles( 0 ),
		mMaxParticles( 0 ),
		mMesh( NULL ),
		mSpawnTime( 0 ),
		mSpawnRate( 0.01f ),
		mIsActive( false )
	{
		Set( renderer, world );
	}

	ParticleSystem::~ParticleSystem()
	{
		for( UINT x = 0; x < (UINT)mEffect.size(); ++x )
			delete mEffect[ x ];

		Shutdown();
	}

	void ParticleSystem::Set( Renderer* renderer, GameWorld* world )
	{
		_ASSERT( renderer );
		_ASSERT( world );

		mRenderer	= renderer;
		mGameWorld	= world;
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

				Particle& particle = GetParticle( mNumParticles );

				particle.mElapsedTime = 0;
				particle.mLifetime    = RandomValue( mMinLifetime, mMaxLifetime );
				particle.mEffectIndex = 0;

				++mNumParticles;
			}

			// Prevent SpawnTime overrun.
			//
			while( mSpawnTime >= mSpawnRate )
				mSpawnTime -= mSpawnRate;
		}

		for( int x = 0; x < (int)mNumParticles; ++x )
		{
			Particle& particle = GetParticle( x );

			// Startup effects in time order.
			//
			for( UINT y = particle.mEffectIndex; y < (UINT)mEffect.size(); ++y )
			{
				if( particle.mElapsedTime >= mEffect[ y ]->mStartTime )
				{
					mEffect[ y ]->Startup( particle );

					++particle.mEffectIndex;
				}
			}

			// Update particles with effects.
			//
			for( UINT y = 0; y < particle.mEffectIndex; ++y )
				mEffect[ y ]->Update( particle );

			particle.Update( DT );

			// Replace dead particles.
			//
			if( particle.mElapsedTime >= particle.mLifetime )
			{
				--mNumParticles;

				GetParticle( x ) = GetParticle( mNumParticles );

				--x;
			}
		}
	}

	void ParticleSystem::Shutdown()
	{
		mNumParticles = 0;

		mIsActive = false;
	}

	void ParticleSystem::SetMatrixWorld( const Matrix4x4& matWorld )
	{
		mMesh->mMatrixWorld = matWorld;
	}

	void ParticleSystem::Save( Archive& archive )
	{
		archive.Write( &mIsActive );
		
		archive.Write( &mNumParticles );
		archive.Write( &mMaxParticles );

		archive.Write( &mSpawnTime );

		archive.Write( &mMinLifetime );
		archive.Write( &mMaxLifetime );
		
		UINT size = mEffect.size();
		archive.Write( &size );

		TRAVERSE_VECTOR( x, mEffect )
		{
			mEffect[ x ]->Save( archive );
		}

		archive.Write( &mSpawnRate );
	}

	void ParticleSystem::Load( Archive& archive )
	{
		archive.Read( &mIsActive );
		
		archive.Read( &mNumParticles );
		archive.Read( &mMaxParticles );

		archive.Read( &mSpawnTime );

		archive.Read( &mMinLifetime );
		archive.Read( &mMaxLifetime );
		
		UINT size = 0;
		archive.Read( &size );

		ParticleEffect* effect;
		for( UINT x = 0; x < size; ++x )
		{
			effect = (ParticleEffect*)SerialRegister::Load( archive );
			mEffect.push_back( effect );
			effect->Load( archive );
		}

		archive.Read( &mSpawnRate );
	}
}
