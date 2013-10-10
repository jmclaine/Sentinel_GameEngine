#pragma once
/*
All GameComponent(s) are freed when their associated GameObject is Shutdown()
Release() should be called manually for a controlled free memory.
*/
#include "WString.h"
#include "Property.h"
#include "GameComponent.h"

using namespace Sentinel::Utilities;

// Create macro to cast the GameComponent to another type.
// Upcasting a WGameComponent to a derived version does
// not work directly, therefore, it is necessary to create
// a new derived version.
//
#define DECLARE_CAST_COMPONENT( refClass )\
	static W##refClass^ Cast( WGameComponent^ component );

#define DEFINE_CAST_COMPONENT_EX( refClass, castClass )\
	W##refClass^ W##refClass::Cast( WGameComponent^ component ) \
	{\
		try {\
			castClass* obj = dynamic_cast< castClass* >( component->GetRef() );\
			return (obj) ? gcnew W##refClass( obj ) : nullptr;\
		} catch( System::Exception^ ) {}\
		return nullptr;\
	}

#define DEFINE_CAST_COMPONENT( refClass )\
	DEFINE_CAST_COMPONENT_EX( refClass, refClass );

namespace Sentinel { namespace Components
{
	ref class WGameObject;

	public enum class ComponentType
	{
		DEFAULT,
		CONTROLLER,
		COLLISION,
		PHYSICS,
		TRANSFORM,
		CAMERA,
		LIGHT,
		DRAWABLE,
		TRIGGER,
	};

	public ref class WGameComponent
	{
		DECLARE_REF_PTR( GameComponent );

	protected:

		WGameComponent();

	public:

		WGameComponent( GameComponent* component );
		virtual ~WGameComponent();

		void			Release();

		virtual System::String^	ToString() override;

		DECLARE_OP_PTR( GameComponent );

		////////////////////////////////

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();

		////////////////////////////////

		void			SetOwner( WGameObject^ owner );

		ComponentType	GetType();

		DECLARE_PROPERTY( System::String^, Name );
	};
}}
