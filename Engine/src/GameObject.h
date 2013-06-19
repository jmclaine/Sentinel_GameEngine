#pragma once

#include <set>
#include <string>

#include "Util.h"
#include "ControllerComponent.h"
#include "PhysicsComponent.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class GameObject
	{
		TransformComponent*					mTransform;
		ControllerComponent*				mController;
		PhysicsComponent*					mPhysics;
		DrawableComponent*					mDrawable;

		std::vector< GameComponent* >		mComponent;

	public:

		GameObject*							mParent;
		std::string							mName;

		//////////////////////////////

		GameObject();

		virtual ~GameObject();

		//////////////////////////////

		GameComponent* AttachComponent( GameComponent* component, const std::string& name );

		void DetachComponent( GameComponent* component );

		//////////////////////////////

		virtual void Startup();

		virtual void UpdateController();

		virtual void UpdatePhysics();

		virtual void UpdateDrawable();

		virtual void UpdateComponents();

		virtual void Shutdown();

		//////////////////////////////

		virtual void OnCollisionEnter( GameObject* obj = NULL );

		virtual void OnCollisionExit( GameObject* obj = NULL );

		//////////////////////////////

		// Store the first occurrence of a component by type.
		//
		GameComponent* FindComponent( ComponentType type );

		// Store all components of a type to the componentList.
		//
		template< class COMPONENT >
		void FindComponent( ComponentType type, std::vector< COMPONENT* >& componentList )
		{
			TRAVERSE_VECTOR( x, mComponent )
			{
				if( mComponent[ x ]->Type() == type )
					componentList.push_back( (COMPONENT*)mComponent[ x ] );
			}
		}
	};
}
