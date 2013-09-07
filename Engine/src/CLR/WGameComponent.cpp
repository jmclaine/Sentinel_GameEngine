#include "WGameComponent.h"
#include "WGameObject.h"
#include "RString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	DEFINE_REF_PTR( GameComponent );

	WGameComponent::WGameComponent()
	{
		mRef = NULL;
	}

	WGameComponent::WGameComponent( GameComponent* component )
	{
		mRef = component;
	}

	WGameComponent::~WGameComponent()
	{}

	void WGameComponent::Delete()
	{
		delete mRef;
	}

	System::String^ WGameComponent::ToString()
	{
		return gcnew System::String( mRef->mName.c_str() );
	}

	DEFINE_OP_PTR( GameComponent );

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

	void WGameComponent::SetOwner( WGameObject^ owner )
	{
		mRef->SetOwner( owner->GetRef() );
	}

	ComponentType WGameComponent::Type()
	{
		return (Sentinel::Components::ComponentType)mRef->Type();
	}

	DEFINE_PROPERTY_S( GameComponent, Name );
}}
