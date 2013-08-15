#pragma once

#include "m_shared_ptr.h"
#include "GameComponent.h"
#include "WString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	ref class WGameObject;

	public enum ComponentType
	{
		COMPONENT_GAME,
		COMPONENT_CONTROLLER,
		COMPONENT_COLLISION,
		COMPONENT_PHYSICS,
		COMPONENT_TRANSFORM,
		COMPONENT_CAMERA,
		COMPONENT_LIGHT,
		COMPONENT_DRAWABLE,
		COMPONENT_TRIGGER,

		NUM_COMPONENTS
	};

	public ref class WGameComponent
	{
	protected:

		GameComponent*	mRef;

		////////////////////////////////

		WGameComponent();

	public:

		WGameComponent( GameComponent* obj );
		WGameComponent( WGameComponent% obj );
		virtual ~WGameComponent();

		GameComponent*	GetRef();

		void			Startup();

		void			Update();

		void			Shutdown();

		////////////////////////////////

		void			SetOwner( WGameObject% owner );

		ComponentType	Type();
		WString^		Name();
	};
}}
