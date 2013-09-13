#pragma once

#include "PerspectiveCameraComponent.h"
#include "WCameraComponent.h"

namespace Sentinel { namespace Components
{
	public ref class WPerspectiveCameraComponent : public WCameraComponent
	{
	protected:

		WPerspectiveCameraComponent();

	public:

		WPerspectiveCameraComponent( float windowWidth, float windowHeight );
		WPerspectiveCameraComponent( PerspectiveCameraComponent* component );

		DECLARE_CAST_COMPONENT( PerspectiveCameraComponent );

		//////////////////////////////

		void	Set( float windowWidth, float windowHeight );
		void	Set( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV );
	};
}}
