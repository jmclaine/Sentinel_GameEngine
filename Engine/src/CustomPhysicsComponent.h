#pragma once
/*
Ensures compatibility with the CLR to save correctly.
*/
#include "PhysicsComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL CustomPhysicsComponent : public PhysicsComponent
	{
	public:

#ifdef IGNORE_PHYSICS
		void*				mIgnoreRB;
#endif
		// These values are required in order to
		// save and load the physics back into
		// BulletPhysics.
		//
		ShapeType			mShapeType;
		float				mMass;

		Vector3f			mShapePosition;
		Quatf				mShapeOrientation;
		Vector3f			mShapeScale;

		int					mFlags;
		float				mLinearDamping;
		float				mAngularDamping;
		float				mRestitution;
		float				mFriction;

		Vector3f			mAngularFactor;
		Vector3f			mGravity;

	public:

		CustomPhysicsComponent();
		~CustomPhysicsComponent();

		void			Startup();

		void			Update();
		
		void			Shutdown();

		/////////////////////////////////

		void			Save( Archive& archive );
		void			Load( Archive& archive );
	};
}
