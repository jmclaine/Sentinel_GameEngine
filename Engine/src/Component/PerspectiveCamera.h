#pragma once

#include "Component/Camera.h"
#include "Vector3f.h"
#include "Vector2f.h"

namespace Sentinel { namespace Component
{
	class SENTINEL_DLL PerspectiveCamera : public Camera
	{
		DECLARE_SERIAL();

	private:

		float			mNearZ;
		float			mFarZ;
		float			mFOV;
		float			mAspectRatio;

		float			mAngle;

	public:

		PerspectiveCamera();
		PerspectiveCamera( UINT windowWidth, UINT windowHeight );
		PerspectiveCamera( UINT windowWidth, UINT windowHeight, float nearZ, float farZ, float FOV );

		void			Startup();

		void			Update();

		void			Shutdown();

		//////////////////////////////

		void			Set( UINT windowWidth, UINT windowHeight );
		void			Set( UINT windowWidth, UINT windowHeight, float nearZ, float farZ, float FOV );

		float			NearZ();
		float			FarZ();
		float			FOV();
		float			AspectRatio();

		float			Angle();

		const Vector3f& LookAt();

		void			GetFrustumSize( Vector3f& nearCenter, Vector3f& farCenter,
										Vector2f& nearSize,   Vector2f& farSize );

		Ray				ScreenPointToRay( UINT mouseX, UINT mouseY, UINT screenWidth = 0, UINT screenHeight = 0 );

		//////////////////////////////

		GameComponent*	Copy();
	};
}}
