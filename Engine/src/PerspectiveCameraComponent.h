#pragma once

#include "CameraComponent.h"

namespace Sentinel
{
	class PerspectiveCameraComponent : public CameraComponent
	{
	public:

		float		mNearZ;
		float		mFarZ;
		float		mFOV;

		vec3f		mLookAt;

		//Plane mPlane[ 6 ];	// view frustum planes

		//////////////////////////////

		PerspectiveCameraComponent( HWND hWnd = NULL, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f );

		void Startup();

		void Update();

		void Shutdown();

		//void setFrustum( HWND hWnd, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f );
		//bool checkFrustum( BoundingSphere& sphere );
		//bool checkFrustum( FrustumBox& box );
	};
}
