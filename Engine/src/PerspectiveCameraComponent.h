#pragma once

#include "Common.h"
#include "CameraComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL PerspectiveCameraComponent : public CameraComponent
	{
	public:

		float		mNearZ;
		float		mFarZ;
		float		mFOV;

		Vector3f	mLookAt;

		//Plane mPlane[ 6 ];	// view frustum planes

		//////////////////////////////

		PerspectiveCameraComponent( float windowWidth, float windowHeight, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f );

		void	Startup();

		void	Update();

		void	Shutdown();

		//////////////////////////////

		void	Set( float windowWidth, float windowHeight, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f );

		//void	CreateFrustum();
		//bool	CheckFrustum( const BoundingSphere& sphere );
		//bool	CheckFrustum( const FrustumBox& box );
	};
}
