#include "OrthographicCameraComponent.h"
#include "TransformComponent.h"
#include "Archive.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( OrthographicCameraComponent );
	DEFINE_SERIAL_CLONE( OrthographicCameraComponent );

	OrthographicCameraComponent::OrthographicCameraComponent()
	{}

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
		CameraComponent::Update();

		mMatrixView.Translate( mTransform->mPosition );
		
		mMatrixFinal = mMatrixProjection * mMatrixView;
	}

	void OrthographicCameraComponent::Shutdown()
	{
		CameraComponent::Shutdown();
	}

	//////////////////////////////

	void OrthographicCameraComponent::Set( float windowWidth, float windowHeight )
	{
		mMatrixProjection.ProjectionOrthographic( windowWidth, windowHeight );
	}

	//////////////////////////////

	void OrthographicCameraComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		archive.Write( mMatrixProjection.Ptr(), 16 );
	}

	void OrthographicCameraComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( mMatrixProjection.Ptr(), 16 );
	}
}
