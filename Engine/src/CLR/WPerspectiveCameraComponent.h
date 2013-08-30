#pragma once

#include "WCameraComponent.h"

namespace Sentinel { namespace Components
{
	public ref class WPerspectiveCameraComponent : public WCameraComponent
	{
	protected:

		WPerspectiveCameraComponent();

	public:

		WPerspectiveCameraComponent( WCameraComponent^ camera );
		WPerspectiveCameraComponent( float windowWidth, float windowHeight );

		void	Startup();

		void	Update();

		void	Shutdown();

		//////////////////////////////

		void	Set( float windowWidth, float windowHeight );
		void	Set( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV );
	};
}}