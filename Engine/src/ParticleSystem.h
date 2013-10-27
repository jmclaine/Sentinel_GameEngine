#pragma once
/*
Creates a particle system based on normal point sprites,
or physics based geometry.

Must create InitialEffect to have a type of PositionEffect
in order to spawn outside the origin.
*/
#include <vector>
#include <memory>

#include "Particle.h"
#include "ParticleEffect.h"
#include "Sprite.h"
#include "Vector3f.h"
#include "Quatf.h"
#include "Matrix4f.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class GameWorld;
	class Renderer;
	class Mesh;
	
	class SENTINEL_DLL ParticleSystem
	{
	protected:

		static Matrix4f		MATRIX_TRANSLATION;
		static Matrix4f		MATRIX_ROTATION;
		static Matrix4f		MATRIX_SCALE;

		bool				mIsActive;

		Renderer*			mRenderer;
		GameWorld*			mWorld;

		UINT				mNumParticles;

		Particle**			mParticle;

		float				mSpawnTime;

	public:

		Mesh*				mMesh;
		
		UINT				mMaxParticles;

		float				mMinLifetime;
		float				mMaxLifetime;
		
		std::vector< ParticleEffect* >	mEffect;	// must be in start time order

		float				mSpawnRate;

	protected:

		//ParticleSystem();
		ParticleSystem( Renderer* renderer, GameWorld* world, UINT maxParticles );

	public:

		virtual ~ParticleSystem();

		virtual void		Startup();

		virtual void		Update( float DT );

		virtual void		Shutdown();
	};

	extern SENTINEL_DLL ParticleSystem* BuildParticleSystemNormal( Renderer* renderer, GameWorld* world, std::shared_ptr< Sprite > sprite, UINT maxParticles = 1000 );
	// extern SENTINEL_DLL ParticleSystem* BuildParticleSystemPhysics( Renderer* renderer, GameWorld* world, Mesh* mesh, UINT maxParticles = 1000 );
}
