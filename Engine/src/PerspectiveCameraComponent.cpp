#include "PerspectiveCameraComponent.h"

namespace Sentinel
{
	PerspectiveCameraComponent::PerspectiveCameraComponent( HWND hWnd, float nearZ, float farZ, float FOV ) :
		mNearZ( nearZ ), mFarZ( farZ ), mFOV( FOV )
	{
		mHWND = hWnd;
		mMatrixProjection.ProjectionPerspective( hWnd, nearZ, farZ, FOV );
	}
	
	void PerspectiveCameraComponent::Startup()
	{
		CameraComponent::Startup();
	}

	void PerspectiveCameraComponent::Update()
	{
		mMatrixView = mTransform->mOrientation.Matrix();
		mLookAt = mTransform->mPosition + mMatrixView.Forward();

		mMatrixView.LookAtView( mTransform->mPosition, mLookAt );
		mMatrixFinal = mMatrixProjection * mMatrixView;
	}

	void PerspectiveCameraComponent::Shutdown()
	{}
}
