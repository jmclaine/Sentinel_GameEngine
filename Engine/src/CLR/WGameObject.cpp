#include "WGameObject.h"
#include "RString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	WGameObject::WGameObject()
	{
		mRef = new GameObject();
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

	DEFINE_PROPERTY_P( GameObject, GameObject, Parent );
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
		 
	void WGameObject::UpdateDrawable()
	{
		mRef->UpdateDrawable();
	}
		 
	void WGameObject::UpdateComponents()
	{
		mRef->UpdateComponents();
	}
		 
	void WGameObject::Shutdown()
	{
		mRef->Shutdown();
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
