#pragma once

#include "Common.h"
#include "CameraComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL PerspectiveCameraComponent : public CameraComponent
	{
	private:

		float			mNearZ;
		float			mFarZ;
		float			mFOV;

		Vector3f		mLookAt;

		//////////////////////////////

		PerspectiveCameraComponent();

	public:

		PerspectiveCameraComponent( float windowWidth, float windowHeight, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f );

		void			Startup();

		void			Update();

		void			Shutdown();

		//////////////////////////////

		void			Set( float windowWidth, float windowHeight, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f );

		float			NearZ();
		float			FarZ();
		float			FOV();
		const Vector3f& LookAt();
	};
}
