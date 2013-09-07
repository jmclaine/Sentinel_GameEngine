#pragma once
/*
All attached GameComponent(s) are freed on Shutdown()
*/
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

		std::vector< GameObject* >			mChild;

	public:

		std::string							mName;
		GameObject*							mParent;

		//////////////////////////////

		GameObject();

		virtual ~GameObject();

		//////////////////////////////

		GameComponent*	AttachComponent( GameComponent* component, const std::string& name );

		GameComponent*	DetachComponent( GameComponent* component );

		//////////////////////////////

		void			AddChild( GameObject* obj );
		void			RemoveChild( UINT index );
		GameObject*		GetChild( UINT index );
		UINT			NumChildren();

		//////////////////////////////

		virtual void	Startup();

		virtual void	UpdateController();

		virtual void	UpdatePhysics();

		virtual void	UpdateTransform();

		virtual void	UpdateComponents();

		virtual void	UpdateDrawable();

		virtual void	Shutdown();

		//////////////////////////////

		// Find the first occurrence of a component by type.
		//
		GameComponent*	FindComponent( ComponentType type );

		// Find all components of a type to the componentList.
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
