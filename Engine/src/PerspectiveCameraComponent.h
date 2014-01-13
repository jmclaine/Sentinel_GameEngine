#pragma once

#include "CameraComponent.h"
#include "Vector3f.h"
#include "Vector2f.h"

namespace Sentinel
{
	class SENTINEL_DLL PerspectiveCameraComponent : public CameraComponent
	{
		DECLARE_SERIAL();

	private:

		float			mNearZ;
		float			mFarZ;
		float			mFOV;
		float			mAspectRatio;

		float			mAngle;

		Vector3f		mLookAt;

	public:

		PerspectiveCameraComponent();
		PerspectiveCameraComponent( float windowWidth, float windowHeight );
		PerspectiveCameraComponent( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV );

		void			Startup();

		void			Update();

		void			Shutdown();

		//////////////////////////////

		void			Set( float windowWidth, float windowHeight );
		void			Set( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV );

		float			NearZ();
		float			FarZ();
		float			FOV();
		float			AspectRatio();

		float			Angle();

		const Vector3f& LookAt();

		void			GetFrustumSize( Vector3f& nearCenter, Vector3f& farCenter,
										Vector2f& nearSize,   Vector2f& farSize );

		Ray				ScreenPointToRay( UINT mouseX, UINT mouseY, UINT screenWidth = 0, UINT screenHeight = 0 );
	};
}
