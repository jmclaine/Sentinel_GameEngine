#pragma once

#include "Common.h"
#include "Vector3f.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class RigidBody;

	// Base particle.
	//
	class SENTINEL_DLL Particle
	{
	public:

		UINT		mEffectIndex;	// required to determine where in the effects this particle should startup

		float		mLifetime;
		float		mElapsedTime;

		Vector3f	mPosition;
		Vector3f	mRotation;
		Vector3f	mScale;

		Vector3f	mVelocity;
		Vector3f	mAngularVelocity;

		Vector3f	mAccel;
		Vector3f	mAngularAccel;

	protected:

		Particle();

	public:

		virtual void Update( float DT );
	};

	// A regular particle without collision.
	//
	class SENTINEL_DLL NormalParticle : public Particle
	{
	public:

		UINT		mFrame;
		ColorRGBA	mColor;

		///////////////////////////////////

		NormalParticle();
	};

	// A physics based particle.
	//
	class SENTINEL_DLL PhysicsParticle : public Particle
	{
	private:

		RigidBody*	mRigidBody;

	public:

		PhysicsParticle();
	};
};