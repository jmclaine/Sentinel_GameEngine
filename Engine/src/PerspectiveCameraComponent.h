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

		Vector3f	mLookAt;

		//Plane mPlane[ 6 ];	// view frustum planes

		//////////////////////////////

		PerspectiveCameraComponent( float windowWidth, float windowHeight, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f );

		void Startup();

		void Update();

		void Shutdown();

		//void CreateFrustum();
		//bool CheckFrustum( const Vector3f position, float radius );
		//bool CheckFrustum( FrustumBox& box );
	};
}
