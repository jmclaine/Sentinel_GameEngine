#pragma once
/*
All GameComponent(s) are freed when their associated GameObject is Shutdown()
Delete() should be called manually for a controlled free memory.
*/
#include "m_shared_ptr.h"
#include "Property.h"
#include "GameComponent.h"

namespace Sentinel { namespace Components
{
	ref class RGameObject;

	public enum class ComponentType
	{
		GAME,
		CONTROLLER,
		COLLISION,
		PHYSICS,
		TRANSFORM,
		CAMERA,
		LIGHT,
		DRAWABLE,
		TRIGGER,

		NUM_COMPONENTS
	};

	public ref class WGameComponent
	{
	protected:

		GameComponent*	mRef;

		////////////////////////////////

		WGameComponent();

	public:

		WGameComponent( GameComponent* component );
		WGameComponent( WGameComponent^ component );
		virtual ~WGameComponent();

		void			Delete();

		GameComponent*	GetRef();

		////////////////////////////////

		void			Startup();

		void			Update();

		void			Shutdown();

		////////////////////////////////

		void			SetOwner( RGameObject^ owner );

		ComponentType	Type();

		DECLARE_PROPERTY( System::String^, Name );
	};
}}
