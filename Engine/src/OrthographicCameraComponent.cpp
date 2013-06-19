#include "OrthographicCameraComponent.h"

namespace Sentinel
{
	OrthographicCameraComponent::OrthographicCameraComponent( HWND hWnd )
	{
		mHWND = hWnd;
	}

	void OrthographicCameraComponent::Startup()
	{
		CameraComponent::Startup();
	}

	void OrthographicCameraComponent::Update()
	{
		mMatrixView.Translate( mTransform->mPosition );
		mMatrixProjection.ProjectionOrthographic( mHWND );
		mMatrixFinal = mMatrixProjection * mMatrixView;
	}

	void OrthographicCameraComponent::Shutdown()
	{}
}
