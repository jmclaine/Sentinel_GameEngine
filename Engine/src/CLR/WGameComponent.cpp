#include "WGameComponent.h"
#include "WGameObject.h"

namespace Sentinel { namespace Components
{
	WGameComponent::WGameComponent()
	{
		mRef = NULL;
	}

	WGameComponent::~WGameComponent()
	{}

	void WGameComponent::Delete()
	{
		delete mRef;
	}

	GameComponent* WGameComponent::GetRef()
	{
		return mRef;
	}

	////////////////////////////////

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

	void WGameComponent::SetOwner( RGameObject^ owner )
	{
		mRef->SetOwner( owner->GetRef() );
	}

	ComponentType WGameComponent::Type()
	{
		return (Sentinel::Components::ComponentType)mRef->Type();
	}

	RString^ WGameComponent::Name()
	{
		return gcnew RString( mRef->mName );
	}
}}
