#pragma once

#include <set>
#include <string>
#include <vector>

#include "Common.h"
#include "Util.h"
#include "GameComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL GameObject
	{
	protected:

		GameComponent*						mTransform;
		GameComponent*						mController;
		GameComponent*						mPhysics;
		GameComponent*						mDrawable;

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
