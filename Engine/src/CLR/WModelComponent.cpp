#include "WModelComponent.h"
#include "ModelComponent.h"

namespace Sentinel { namespace Components
{
	WModelComponent::WModelComponent( WModel^ model )
	{
		mRef = new ModelComponent( model->GetRef() );
	}

	WModelComponent::WModelComponent( ModelComponent* component )
	{
		mRef = component;
	}

	DEFINE_CAST_COMPONENT( ModelComponent );

	DEFINE_PROPERTY_PS( ModelComponent, Model, Model );

	void WModelComponent::SetMaterial( WMaterial^ material )
	{
		static_cast< ModelComponent* >(mRef)->SetMaterial( material );
	}

	void WModelComponent::SetMaterial( int index, WMaterial^ material )
	{
		_ASSERT( index >= 0 );

		static_cast< ModelComponent* >(mRef)->SetMaterial( (UINT)index, material );
	}

	void WModelComponent::SetMaterial( WStdVector_Material^ material )
	{
		static_cast< ModelComponent* >(mRef)->SetMaterial( material );
	}

	WStdVector_Material^ WModelComponent::GetMaterial()
	{
		return gcnew WStdVector_Material( static_cast< ModelComponent* >(mRef)->GetMaterial() );
	}

	int WModelComponent::GetMaterialSize()
	{
		return (int)static_cast< ModelComponent* >(mRef)->GetMaterialSize();
	}
}}
