#include "WGameObject.h"
#include "RString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
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

	void WGameObject::Delete()
	{
		delete mRef;
	}

	String^ WGameObject::ToString()
	{
		return gcnew String( mRef->mName.c_str() );
	}

	DEFINE_OP_PTR( GameObject );

	////////////////////////////////

	DEFINE_PROPERTY_STR( GameObject, Name );
	DEFINE_PROPERTY_PS( GameObject, GameObject, Parent );

	////////////////////////////////

	WGameComponent^ WGameObject::AttachComponent( WGameComponent^ component, String^ name )
	{
		mRef->AttachComponent( component, RString::ToString( name ));
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
	WGameComponent^ WGameObject::FindComponent( Sentinel::Components::ComponentType type )
	{
		GameComponent* component = mRef->FindComponent( (Sentinel::ComponentType)type );

		return (component) ? gcnew WGameComponent( component ) : nullptr;
	}
}}
