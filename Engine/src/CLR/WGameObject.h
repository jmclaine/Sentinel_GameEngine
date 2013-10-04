#pragma once
/*
Creates a new GameObject.
WGameObject frees automatically when attached to WGameWorld.
Delete() should be called manually for a controlled free memory.
*/
#include "Property.h"
#include "GameObject.h"
#include "WGameComponent.h"

namespace Sentinel { namespace Components
{
	public ref class WGameObject
	{
		DECLARE_REF_PTR( GameObject );
	
	public:

		WGameObject();
		WGameObject( GameObject* obj );

		void Delete();

		virtual System::String^	ToString() override;

		DECLARE_OP_PTR( GameObject );

		////////////////////////////////

		DECLARE_PROPERTY( System::String^,		Name );
		DECLARE_PROPERTY( WGameObject^, Parent );

		////////////////////////////////

		WGameComponent^ AttachComponent( WGameComponent^ component, System::String^ name );

		WGameComponent^	DetachComponent( WGameComponent^ component );

		////////////////////////////////

		void			AddChild( WGameObject^ obj );
		void			RemoveChild( WGameObject^ obj );
		WGameObject^	GetChild( UINT index );
		UINT			NumChildren();

		////////////////////////////////

		virtual void	Startup();

		virtual void	UpdateController();

		virtual void	UpdatePhysics();

		virtual void	UpdateTransform();

		virtual void	UpdateComponents();

		virtual void	UpdateDrawable();
		virtual void	UpdateDrawable( bool drawChildren );

		virtual void	Shutdown();

		//////////////////////////////

		// Find the first occurrence of a component by type.
		//
		WGameComponent^ FindComponent( Sentinel::Components::ComponentType type );
	};
}}
