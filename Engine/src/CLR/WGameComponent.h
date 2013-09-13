#pragma once
/*
All GameComponent(s) are freed when their associated GameObject is Shutdown()
Delete() should be called manually for a controlled free memory.
*/
#include "m_shared_ptr.h"
#include "Property.h"
#include "GameComponent.h"

// Create macro to cast the GameComponent to another type.
// Upcasting a WGameComponent to a derived version does
// not work directly, therefore, it is necessary to create
// a new derived version.
//
#define DECLARE_CAST_COMPONENT( refClass )\
	static W##refClass^ Cast( WGameComponent^ component );

#define DEFINE_CAST_COMPONENT( refClass )\
	W##refClass^ W##refClass::Cast( WGameComponent^ component ) \
	{\
		refClass* obj = dynamic_cast< refClass* >( component->GetRef() );\
		return (obj) ? gcnew W##refClass( obj ) : nullptr;\
	}

namespace Sentinel { namespace Components
{
	ref class WGameObject;

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
		DECLARE_REF_PTR( GameComponent );

	protected:

		WGameComponent();

	public:

		WGameComponent( GameComponent* component );
		virtual ~WGameComponent();

		void			Delete();

		virtual System::String^	ToString() override;

		DECLARE_OP_PTR( GameComponent );

		////////////////////////////////

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();

		////////////////////////////////

		void			SetOwner( WGameObject^ owner );

		ComponentType	Type();

		DECLARE_PROPERTY( System::String^, Name );
	};
}}
