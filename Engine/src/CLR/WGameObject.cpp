#include "WGameObject.h"
#include "WString.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_PTR( GameObject );

	WGameObject::WGameObject()
	{
		mRef = new GameObject();
	}

	WGameObject::WGameObject( GameObject* obj )
	{
		mRef = obj;
	}

	void WGameObject::Release()
	{
		delete mRef;
	}

	System::String^ WGameObject::ToString()
	{
		return gcnew System::String( mRef->mName.c_str() );
	}

	DEFINE_OP_PTR( GameObject );

	////////////////////////////////

	DEFINE_PROPERTY_STR( GameObject, Name );
	DEFINE_PROPERTY_PFG( GameObject, GameObject, Parent );

	////////////////////////////////

	WGameComponent^ WGameObject::AttachComponent( WGameComponent^ component, System::String^ name )
	{
		mRef->AttachComponent( component, WString::Alloc( name ));
		return component;
	}

	WGameComponent^ WGameObject::DetachComponent( WGameComponent^ component )
	{
		mRef->DetachComponent( component );
		return component;
	}

	////////////////////////////////

	void WGameObject::AddChild( WGameObject^ obj )
	{
		mRef->AddChild( obj );
	}

	void WGameObject::RemoveChild( WGameObject^ obj )
	{
		mRef->RemoveChild( obj );
	}

	WGameObject^ WGameObject::GetChild( UINT index )
	{
		return gcnew WGameObject( mRef->GetChild( index ));
	}

	UINT WGameObject::NumChildren()
	{
		return mRef->NumChildren();
	}

	////////////////////////////////

	void WGameObject::Startup()
	{
		mRef->Startup();
	}
		 
	void WGameObject::UpdateController()
	{
		mRef->UpdateController();
	}
		 
	void WGameObject::UpdatePhysics()
	{
		mRef->UpdatePhysics();
	}
	
	void WGameObject::UpdateTransform()
	{
		mRef->UpdateTransform();
	}

	void WGameObject::UpdateComponents()
	{
		mRef->UpdateComponents();
	}

	void WGameObject::UpdateDrawable()
	{
		mRef->UpdateDrawable();
	}

	void WGameObject::UpdateDrawable( bool drawChildren )
	{
		mRef->UpdateDrawable( drawChildren );
	}
		 
	void WGameObject::Shutdown()
	{
		mRef->Shutdown();
	}

	//////////////////////////////

	// Find the first occurrence of a component by type.
	//
	WGameComponent^ WGameObject::FindComponent( Sentinel::Wrapped::ComponentType type )
	{
		GameComponent* component = mRef->FindComponent( (Sentinel::GameComponent::Type)type );

		return (component) ? gcnew WGameComponent( component ) : nullptr;
	}
}}
