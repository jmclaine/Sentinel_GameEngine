#pragma once
/*
All attached GameComponent(s) are freed on Shutdown()
*/
#include <set>
#include <vector>

#include "Util.h"
#include "GameComponent.h"
#include "ListNode.h"

namespace Sentinel
{
	class GameWorld;

	class SENTINEL_DLL GameObject : public Serializable, public ListNode< GameObject >
	{
	protected:

		DECLARE_SERIAL();

		GameComponent*						mTransform;
		GameComponent*						mController;
		GameComponent*						mPhysics;
		GameComponent*						mDrawable;

		std::vector< GameComponent* >		mComponent;

		GameWorld*							mWorld;

		bool								mIsActive;

	public:

		std::string							mName;
		
		//////////////////////////////

		GameObject();

		virtual ~GameObject();

		//////////////////////////////

		GameComponent*	AttachComponent( GameComponent* component );
		GameComponent*	DetachComponent( GameComponent* component );

		//////////////////////////////

		GameObject*		AddChild( GameObject* obj );	// override

		//////////////////////////////

		GameWorld*		GetWorld();
		void			SetWorld( GameWorld* world );

		void			SetActive( bool active );

		//////////////////////////////

		virtual void	Startup();

		virtual void	UpdateController();

		virtual void	UpdatePhysics();

		virtual void	UpdateTransform();

		virtual void	UpdateComponents();

		virtual void	UpdateDrawable( bool drawChildren = true );

		virtual void	Shutdown();

		//////////////////////////////

		// Find the first occurrence of a component by type.
		//
		GameComponent*	FindComponent( GameComponent::Type type );

		// Find all components of a type to the componentList.
		//
		template< class COMPONENT >
		void FindComponent( GameComponent::Type type, std::vector< COMPONENT* >& componentList )
		{
			TRAVERSE_VECTOR( x, mComponent )
			{
				if( mComponent[ x ]->GetType() == type )
					componentList.push_back( (COMPONENT*)mComponent[ x ] );
			}
		}
	};
}
