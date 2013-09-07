#include "WModelComponent.h"
#include "ModelComponent.h"

namespace Sentinel { namespace Components
{
	WModelComponent::WModelComponent( WModel^ model )
	{
		mRef = new ModelComponent( model->GetRef() );
	}

	DEFINE_CAST_COMPONENT( ModelComponent );
}}
