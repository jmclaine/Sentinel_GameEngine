#pragma once

#include "CameraComponent.h"
#include "WTransformComponent.h"

namespace Sentinel { namespace Components
{
	public ref class WCameraComponent : public WGameComponent
	{
	protected:

		WCameraComponent();

	public:

		WCameraComponent( CameraComponent* camera );

		const WTransformComponent^	GetTransform();
	};
}}
