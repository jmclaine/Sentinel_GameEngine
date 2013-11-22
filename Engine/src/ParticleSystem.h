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
#include "Serializable.h"
#include "Vector3f.h"
#include "Quatf.h"
#include "Matrix4f.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class GameWorld;
	class Renderer;
	class Mesh;
	class Sprite;
	class Archive;
	
	class SENTINEL_DLL ParticleSystem : public Serializable
	{
	public:

		enum Type
		{
			NORMAL,
			PHYSICS,
		};

	protected:

		bool				mIsActive;

		Renderer*			mRenderer;
		GameWorld*			mGameWorld;

		UINT				mNumParticles;
		UINT				mMaxParticles;

		float				mSpawnTime;

		std::shared_ptr< Mesh > mMesh;

	public:

		float				mMinLifetime;
		float				mMaxLifetime;
		
		std::vector< ParticleEffect* >	mEffect;	// must be in start time order

		float				mSpawnRate;

	protected:

		ParticleSystem();
		ParticleSystem( Renderer* renderer, GameWorld* world );
		
	public:

		virtual ~ParticleSystem();

		void				Set( Renderer* renderer, GameWorld* world );

		///////////////////////////////////

		virtual void		Startup();

		virtual void		Update( float DT );

		virtual void		Shutdown();

		///////////////////////////////////

		void				SetMatrixWorld( const Matrix4f& matWorld );

	private:

		virtual Particle&	GetParticle( UINT index ) = 0;

	public:

		virtual void		Save( Archive& archive );

		virtual void		Load( Archive& archive );
	};

	extern SENTINEL_DLL ParticleSystem* BuildParticleSystemNormal( Renderer* renderer, GameWorld* world, std::shared_ptr< Sprite > sprite, UINT maxParticles );
	extern SENTINEL_DLL ParticleSystem* BuildParticleSystemNormal( Renderer* renderer, GameWorld* world );

	// extern SENTINEL_DLL ParticleSystem* BuildParticleSystemPhysics( Renderer* renderer, GameWorld* world, std::shared_ptr< Mesh > mesh, UINT maxParticles );
}
