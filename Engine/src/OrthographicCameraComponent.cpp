#include "OrthographicCameraComponent.h"
#include "TransformComponent.h"
#include "Archive.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameWorld.h"

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

	Ray OrthographicCameraComponent::ScreenPointToRay( UINT mouseX, UINT mouseY, UINT screenWidth, UINT screenHeight )
	{
		return Ray( Vector3f( (float)mouseX, (float)mouseY, 0 ), Vector3f( 0, 0, -1 ));
	}

	//////////////////////////////

	void OrthographicCameraComponent::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );

		GameComponent::Save( archive );

		archive.Write( mMatrixProjection.Ptr(), 16 );
	}

	void OrthographicCameraComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( mMatrixProjection.Ptr(), 16 );
	}
}
