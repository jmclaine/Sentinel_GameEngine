#include "WGameObject.h"
#include "WString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	WGameObject::WGameObject()
	{
		mRef = new GameObject();
	}

	WGameObject::WGameObject( GameObject* obj )
	{
		mRef = obj;
	}

	WGameObject::WGameObject( WGameObject% obj )
	{
		mRef = obj.mRef;
	}

	WGameObject::~WGameObject()
	{
		delete mRef;
	}

	WGameObject::!WGameObject()
	{
		delete mRef;
	}

	GameObject* WGameObject::GetRef()
	{
		return mRef;
	}

	WGameObject^ WGameObject::Parent()
	{
		return gcnew WGameObject( mRef->mParent );
	}

	WString^ WGameObject::Name()
	{
		return gcnew WString( mRef->mName );
	}

	//////////////////////////////

	WGameComponent^ WGameObject::AttachComponent( WGameComponent^ component, System::String^ name )
	{
		return gcnew WGameComponent( mRef->AttachComponent( component->GetRef(), WString::ToString( name )));
	}

	void WGameObject::DetachComponent( WGameComponent^ component )
	{
		mRef->DetachComponent( component->GetRef() );
	}

	//////////////////////////////

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
}}
