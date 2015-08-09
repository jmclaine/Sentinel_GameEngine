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
	namespace Component
	{
		class Camera;
	}

	class GameWorld;

	class SENTINEL_DLL GameObject : public Serializable, public ListNode< GameObject >
	{
	private:

		DECLARE_SERIAL();

		GameComponent*						mTransform;
		GameComponent*						mController;
		GameComponent*						mPhysics;
		GameComponent*						mDrawable;
		GameComponent*						mCamera;
		std::vector< GameComponent* >		mComponent;

		GameWorld*							mWorld;

		bool								mEnabled;

	public:

		std::string							mName;
		WORD								mLayer;
		
		//////////////////////////////

		GameObject( const std::string& name = "GameObject" );
		~GameObject();

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

		void			Startup();

		void			UpdateController();
		void			UpdatePhysics();
		void			UpdateTransform();
		void			UpdateComponents();
		void			UpdateCamera();
		void			UpdateDrawable();

		void			Shutdown();

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

		//////////////////////////////

		GameObject*		Copy();
	};
}
