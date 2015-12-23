#pragma once

#include "Sentinel.h"
#include "Vector3.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class RigidBody;

	// Base particle.
	//
	class SENTINEL_DLL Particle
	{
	public:

		UINT		mEffectIndex;	// determines where in the effects this particle should startup

		float		mLifetime;
		float		mElapsedTime;

		Vector3	mPosition;
		Vector3	mRotation;
		Vector3	mScale;

		Vector3	mVelocity;
		Vector3	mAngularVelocity;

		Vector3	mAccel;
		Vector3	mAngularAccel;

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