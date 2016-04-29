#pragma once

#include "Sentinel.h"
#include "Vector3.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class RigidBody;

	class SENTINEL_DLL Particle
	{
	public:

		UINT mEffectIndex;

		float mLifetime;
		float mElapsedTime;

		Vector3 mPosition;
		Vector3 mRotation;
		Vector3 mScale;

		Vector3 mVelocity;
		Vector3 mAngularVelocity;

		Vector3 mAccel;
		Vector3 mAngularAccel;

	protected:

		Particle();

	public:

		virtual void Update(float DT);
	};

	class SENTINEL_DLL SpriteParticle : public Particle
	{
	public:

		UINT mFrame;
		ColorRGBA mColor;

		///////////////////////////////////

		SpriteParticle();
	};

	class SENTINEL_DLL PhysicsParticle : public Particle
	{
	private:

		RigidBody* mRigidBody;

	public:

		PhysicsParticle();
	};
};