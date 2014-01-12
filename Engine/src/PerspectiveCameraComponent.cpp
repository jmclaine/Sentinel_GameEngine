#include "PerspectiveCameraComponent.h"
#include "TransformComponent.h"
#include "Archive.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "MathCommon.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( PerspectiveCameraComponent );
	DEFINE_SERIAL_CLONE( PerspectiveCameraComponent );

	PerspectiveCameraComponent::PerspectiveCameraComponent() :
		mNearZ( 0.3f ),
		mFarZ( 1000.0f ),
		mFOV( 60.0f )
	{
		Set( 1920, 1080 );
	}

	PerspectiveCameraComponent::PerspectiveCameraComponent( float windowWidth, float windowHeight ) :
		mNearZ( 0.3f ),
		mFarZ( 1000.0f ),
		mFOV( 60.0f )
	{
		Set( windowWidth, windowHeight );
	}

	PerspectiveCameraComponent::PerspectiveCameraComponent( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV )
	{
		Set( windowWidth, windowHeight, nearZ, farZ, FOV );
	}
	
	void PerspectiveCameraComponent::Startup()
	{
		CameraComponent::Startup();
	}

	void PerspectiveCameraComponent::Update()
	{
		CameraComponent::Update();

		WindowInfo* info = mOwner->GetWorld()->mRenderer->GetWindow();

		if( mScaleToWindow )
		{
			Set( (float)info->Width(), (float)info->Height(), mNearZ, mFarZ, mFOV );
		}
		else
		{
			Set( (float)Renderer::WINDOW_WIDTH_BASE, (float)Renderer::WINDOW_HEIGHT_BASE, mNearZ, mFarZ, mFOV );
		}

		mMatrixView.Rotate( mTransform->mOrientation );
		mLookAt = mTransform->mPosition + mMatrixView.Forward();

		mMatrixView.LookAtView( mTransform->mPosition, mLookAt, mMatrixView.Up() );
		mMatrixFinal = mMatrixProjection * mMatrixView;
	}

	void PerspectiveCameraComponent::Shutdown()
	{
		CameraComponent::Shutdown();
	}

	//////////////////////////////

	void PerspectiveCameraComponent::Set( float windowWidth, float windowHeight )
	{
		Set( windowWidth, windowHeight, mNearZ, mFarZ, mFOV );
	}

	void PerspectiveCameraComponent::Set( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV )
	{
		mNearZ = nearZ;
		mFarZ  = farZ;
		mFOV   = FOV;

		mAspectRatio = windowWidth / windowHeight;

		mMatrixProjection.ProjectionPerspective( windowWidth, windowHeight, nearZ, farZ, FOV );
	}

	float PerspectiveCameraComponent::NearZ()
	{
		return mNearZ;
	}

	float PerspectiveCameraComponent::FarZ()
	{
		return mFarZ;
	}

	float PerspectiveCameraComponent::FOV()
	{
		return mFOV;
	}

	float PerspectiveCameraComponent::AspectRatio()
	{
		return mAspectRatio;
	}

	const Vector3f& PerspectiveCameraComponent::LookAt()
	{
		return mLookAt;
	}

	void PerspectiveCameraComponent::GetFrustumSize( Vector3f& nearCenter, Vector3f& farCenter,
													 Vector2f& nearSize,   Vector2f& farSize )
	{
		const Matrix4f& mat = GetTransform()->GetMatrixWorld();

		Vector3f right = mat.Right();
		Vector3f up    = mat.Up();
		Vector3f dir   = mat.Forward();

		Vector3f pos = GetTransform()->mPosition;

		nearCenter = pos + dir * NearZ();
		farCenter  = pos + dir * FarZ();

		float angle = tan( (float)DEGREES_TO_RADIANS * FOV() * 0.5f );

		float nearHeight = NearZ() * angle;
		float farHeight  = FarZ()  * angle;

		nearSize = Vector2f( nearHeight * AspectRatio(), nearHeight );
		farSize  = Vector2f( farHeight * AspectRatio(), farHeight );
	}

	Ray PerspectiveCameraComponent::ScreenPointToRay( UINT mouseX, UINT mouseY, UINT screenWidth, UINT screenHeight )
	{
		const Matrix4f& cameraMatrix = GetTransform()->GetMatrixWorld();

		Vector3f nearCenter, farCenter;
		Vector2f nearSize, farSize;

		if( screenWidth == 0 )
			screenWidth = Renderer::WINDOW_WIDTH_BASE;

		if( screenHeight == 0 )
			screenHeight = Renderer::WINDOW_HEIGHT_BASE;

		GetFrustumSize( nearCenter, farCenter, nearSize, farSize );

		float ratioX = -2.0f * (0.5f - (mouseX / static_cast< float >(screenWidth)));
		float ratioY = 2.0f * (0.5f - (mouseY / static_cast< float >(screenHeight)));

		Vector3f nearPos( nearCenter + cameraMatrix.Right() * ratioX * nearSize.x + cameraMatrix.Up() * ratioY * nearSize.y );
		Vector3f farPos( farCenter + cameraMatrix.Right() * ratioX * farSize.x + cameraMatrix.Up() * ratioY * farSize.y );

		//return Ray( nearPos, (nearPos - mTransform->mPosition).Normalize() );
		return Ray( nearPos, (farPos - nearPos).Normalize() );
	}

	//////////////////////////////

	void PerspectiveCameraComponent::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );

		GameComponent::Save( archive );

		archive.Write( mMatrixProjection.Ptr(), 16 );
		archive.Write( &mNearZ );
		archive.Write( &mFarZ );
		archive.Write( &mFOV );
	}

	void PerspectiveCameraComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( mMatrixProjection.Ptr(), 16 );
		archive.Read( &mNearZ );
		archive.Read( &mFarZ );
		archive.Read( &mFOV );
	}
}
