#pragma once

#include "GameComponent.h"

namespace Sentinel
{
	class RigidBody;

namespace Component
{
	class Transform;

	class SENTINEL_DLL Physics : public GameComponent
	{
		DECLARE_SERIAL();
		
	protected:

		Transform*		mTransform;

		RigidBody*		mRigidBody;

	public:

		Physics();
		Physics( RigidBody* body );
		~Physics();

		void			Startup();

		void			Update();

		void			Shutdown();

		/////////////////////////////////

		RigidBody*		GetRigidBody();

		// Startup must be called to add the RigidBody
		// to the PhysicsSystem within GameWorld.
		//
		void			SetRigidBody( RigidBody* body );

		/////////////////////////////////

		GameComponent*	Copy();
	};
}}
