#pragma once

#include "WCameraComponent.h"

namespace Sentinel { namespace Components
{
	public ref class WPerspectiveCameraComponent : public WCameraComponent
	{
	protected:

		WPerspectiveCameraComponent();

	public:

		WPerspectiveCameraComponent( float windowWidth, float windowHeight );

		DECLARE_CAST_COMPONENT( WPerspectiveCameraComponent );

		void	Startup();

		void	Update();

		void	Shutdown();

		//////////////////////////////

		void	Set( float windowWidth, float windowHeight );
		void	Set( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV );
	};
}}
