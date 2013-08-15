#include "OrthographicCameraComponent.h"

namespace Sentinel
{
	OrthographicCameraComponent::OrthographicCameraComponent( float windowWidth, float windowHeight )
	{
		Set( windowWidth, windowHeight );
	}

	void OrthographicCameraComponent::Startup()
	{
		CameraComponent::Startup();
	}

	void OrthographicCameraComponent::Update()
	{
		mMatrixView.Translate( GetTransform()->mPosition );
		
		mMatrixFinal = mMatrixProjection * mMatrixView;
	}

	void OrthographicCameraComponent::Shutdown()
	{}

	//////////////////////////////

	void OrthographicCameraComponent::Set( float windowWidth, float windowHeight )
	{
		mMatrixProjection.ProjectionOrthographic( windowWidth, windowHeight );
	}
}
