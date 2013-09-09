#pragma once

#include "ModelComponent.h"
#include "WDrawableComponent.h"
#include "WModel.h"

using namespace Sentinel::Assets;

namespace Sentinel { namespace Components
{
	public ref class WModelComponent : public WDrawableComponent
	{
	public:

		WModelComponent( WModel^ model );
		WModelComponent( ModelComponent* component );

		DECLARE_CAST_COMPONENT( ModelComponent );

		DECLARE_PROPERTY( WModel^, Model );

		void	SetMaterial( WMaterial^ material );
		void	SetMaterial( int index, WMaterial^ material );
		void	SetMaterial( WStdVector_Material^ material );
		WStdVector_Material^ GetMaterial();
		int		GetMaterialSize();
	};
}}
