#include "WGameComponent.h"
#include "WGameObject.h"
#include "RString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	WGameComponent::WGameComponent()
	{
		mRef = NULL;
	}

	WGameComponent::WGameComponent( GameComponent* component )
	{
		mRef = component;
	}

	WGameComponent::WGameComponent( WGameComponent^ component )
	{
		mRef = component->mRef;
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

	DEFINE_PROPERTY_S( GameComponent, Name );
}}
