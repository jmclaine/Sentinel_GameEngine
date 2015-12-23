#include "Particle.h"

namespace Sentinel
{
	Particle::Particle() :
		mEffectIndex( 0 ),
		mLifetime( 0 ),
		mElapsedTime( 0 ),
		mScale( Vector3( 1, 1, 1 ))
	{}

	void Particle::Update( float DT )
	{
		mElapsedTime += DT;
		
		mVelocity += mAccel * DT;
		mPosition += mVelocity * DT;

		mAngularVelocity += mAngularAccel * DT;
		mRotation += mAngularVelocity * DT;
	}

	//////////////////////////////////////////////////////////////////////

	NormalParticle::NormalParticle() :
		mFrame( 0 )
	{}

	//////////////////////////////////////////////////////////////////////

	PhysicsParticle::PhysicsParticle()
	{}
}
