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

	DEFINE_PROPERTY_S(  GameObject, Name );
	DEFINE_PROPERTY_PS( GameObject, GameObject, Parent );

	////////////////////////////////

	WGameComponent^ WGameObject::AttachComponent( WGameComponent^ component, String^ name )
	{
		mRef->AttachComponent( component->GetRef(), RString::ToString( name ));
		return component;
	}

	WGameComponent^ WGameObject::DetachComponent( WGameComponent^ component )
	{
		mRef->DetachComponent( component->GetRef() );
		return component;
	}

	////////////////////////////////

	void WGameObject::AddChild( WGameObject^ obj )
	{
		mRef->AddChild( obj->GetRef() );
	}

	void WGameObject::RemoveChild( int index )
	{
		_ASSERT( index >= 0 );

		mRef->RemoveChild( (UINT)index );
	}

	WGameObject^ WGameObject::GetChild( int index )
	{
		_ASSERT( index >= 0 );

		GameObject* obj = mRef->GetChild( (UINT)index );
		return gcnew WGameObject( obj );
	}

	int WGameObject::NumChildren()
	{
		return (int)mRef->NumChildren();
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
