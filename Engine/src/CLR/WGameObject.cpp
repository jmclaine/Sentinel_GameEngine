#include "WGameObject.h"
#include "RString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	WGameObject::WGameObject()
	{
		mRef  = new GameObject();
	}

	WGameObject::~WGameObject()
	{
		Delete();
	}

	void WGameObject::Delete()
	{
		delete mRef;
	}

	GameObject* WGameObject::GetRef()
	{
		return mRef;
	}

	System::String^ WGameObject::ToString()
	{
		return gcnew System::String( mRef->mName.c_str() );
	}

	////////////////////////////////

	DEFINE_PROPERTY_S( GameObject, Name );

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

	WGameObject^ WGameObject::GetParent()
	{
		return gcnew RGameObject( mRef->GetParent() );
	}

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

		return gcnew RGameObject( mRef->GetChild( (UINT)index ));
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
		return gcnew WGameComponent( mRef->FindComponent( (Sentinel::ComponentType)type ));
	}

	////////////////////////////////

	RGameObject::RGameObject( GameObject* obj )
	{
		mRef = obj;
	}

	RGameObject::RGameObject( WGameObject^ obj )
	{
		mRef = obj->GetRef();
	}
}}
