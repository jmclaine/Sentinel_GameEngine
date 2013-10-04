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
#include "Serializeable.h"

namespace Sentinel
{
	class SENTINEL_DLL GameObject : public Serializeable
	{
		friend class GameWorld;

		DECLARE_SERIAL();

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

		GameComponent*	AttachComponent( GameComponent* component );
		GameComponent*	AttachComponent( GameComponent* component, const char* name );

		GameComponent*	DetachComponent( GameComponent* component );

		//////////////////////////////

		void			AddChild( GameObject* obj );

		void			RemoveChild( GameObject* obj );

		GameObject*		GetChild( UINT index );

		UINT			NumChildren();

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
