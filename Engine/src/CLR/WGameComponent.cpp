#include "WGameComponent.h"
#include "WGameObject.h"

namespace Sentinel { namespace Components
{
	WGameComponent::WGameComponent()
	{
		mRef = NULL;
	}

	WGameComponent::WGameComponent( GameComponent* obj )
	{
		mRef = obj;
	}

	WGameComponent::WGameComponent( WGameComponent% obj )
	{
		mRef = obj.mRef;
	}

	WGameComponent::~WGameComponent()
	{}

	GameComponent* WGameComponent::GetRef()
	{
		return mRef;
	}

	void WGameComponent::Startup()
	{
		mRef->Startup();
	}

	void WGameComponent::Update()
	{
		mRef->Update();
	}

	void WGameComponent::Shutdown()
	{
		mRef->Shutdown();
	}

	void WGameComponent::SetOwner( WGameObject% owner )
	{
		mRef->SetOwner( owner.GetRef() );
	}

	ComponentType WGameComponent::Type()
	{
		return (Sentinel::Components::ComponentType)mRef->Type();
	}

	WString^ WGameComponent::Name()
	{
		return gcnew WString( mRef->mName );
	}
}}
