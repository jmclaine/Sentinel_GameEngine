#pragma once

#include "CameraComponent.h"
#include "Vector3f.h"

namespace Sentinel
{
	class SENTINEL_DLL PerspectiveCameraComponent : public CameraComponent
	{
		DECLARE_SERIAL();

	private:

		float			mNearZ;
		float			mFarZ;
		float			mFOV;

		Vector3f		mLookAt;

	public:

		PerspectiveCameraComponent();
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
