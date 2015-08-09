#include "Component/PerspectiveCamera.h"
#include "Component/Transform.h"
#include "Archive.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "MathCommon.h"

namespace Sentinel { namespace Component
{
	DEFINE_SERIAL_REGISTER( PerspectiveCamera );
	DEFINE_SERIAL_CLONE( PerspectiveCamera );

	PerspectiveCamera::PerspectiveCamera() :
		mNearZ( 0.3f ),
		mFarZ( 1000.0f ),
		mFOV( 60.0f )
	{
		Set( 1920, 1080 );
	}

	PerspectiveCamera::PerspectiveCamera( UINT windowWidth, UINT windowHeight ) :
		mNearZ( 0.3f ),
		mFarZ( 1000.0f ),
		mFOV( 60.0f )
	{
		Set( windowWidth, windowHeight );
	}

	PerspectiveCamera::PerspectiveCamera( UINT windowWidth, UINT windowHeight, float nearZ, float farZ, float FOV )
	{
		Set( windowWidth, windowHeight, nearZ, farZ, FOV );
	}
	
	void PerspectiveCamera::Startup()
	{
		Camera::Startup();
	}

	void PerspectiveCamera::Update()
	{
		Camera::Update();

		const Matrix4f& cameraMatrix = GetMatrixWorld();
		Vector3f nearCenter, farCenter;
		Vector2f nearSize, farSize;

		GetFrustumSize( nearCenter, farCenter, nearSize, farSize );
		mFrustum.Set( nearCenter, farCenter, nearSize, farSize, cameraMatrix.Forward(), cameraMatrix.Right(), cameraMatrix.Up() );
	}

	void PerspectiveCamera::Shutdown()
	{
		Camera::Shutdown();
	}

	//////////////////////////////

	void PerspectiveCamera::Set( UINT windowWidth, UINT windowHeight )
	{
		Set( windowWidth, windowHeight, mNearZ, mFarZ, mFOV );
	}

	void PerspectiveCamera::Set( UINT windowWidth, UINT windowHeight, float nearZ, float farZ, float FOV )
	{
		mNearZ = nearZ;
		mFarZ  = farZ;
		mFOV   = FOV;

		mAspectRatio  = windowWidth / static_cast< float >(windowHeight);

		mAngle = tan( (float)DEGREES_TO_RADIANS * mFOV * 0.5f );

		mMatrixProjection.ProjectionPerspective( (float)windowWidth, (float)windowHeight, nearZ, farZ, FOV );
	}

	float PerspectiveCamera::NearZ()
	{
		return mNearZ;
	}

	float PerspectiveCamera::FarZ()
	{
		return mFarZ;
	}

	float PerspectiveCamera::FOV()
	{
		return mFOV;
	}

	float PerspectiveCamera::AspectRatio()
	{
		return mAspectRatio;
	}

	float PerspectiveCamera::Angle()
	{
		return mAngle;
	}

	const Vector3f& PerspectiveCamera::LookAt()
	{
		return mLookAt;
	}

	void PerspectiveCamera::GetFrustumSize( Vector3f& nearCenter, Vector3f& farCenter,
											Vector2f& nearSize,   Vector2f& farSize )
	{
		const Matrix4f& mat = GetMatrixWorld();

		Vector3f pos = GetTransform()->mPosition;
		Vector3f dir = mat.Forward();

		nearCenter = pos + dir * NearZ();
		farCenter  = pos + dir * FarZ();

		float nearHeight = NearZ() * Angle();
		float farHeight  = FarZ()  * Angle();

		nearSize = Vector2f( nearHeight * AspectRatio(), nearHeight );
		farSize  = Vector2f( farHeight * AspectRatio(), farHeight );
	}

	Ray PerspectiveCamera::ScreenPointToRay( UINT mouseX, UINT mouseY, UINT screenWidth, UINT screenHeight )
	{
		const Matrix4f& cameraMatrix = GetMatrixWorld();

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
		
		return Ray( nearPos, nearPos - GetTransform()->mPosition );
	}

	//////////////////////////////

	void PerspectiveCamera::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );

		Camera::Save( archive );

		archive.Write( mMatrixProjection.Ptr(), ar_sizeof( mMatrixProjection ));
		archive.Write( &mNearZ );
		archive.Write( &mFarZ );
		archive.Write( &mFOV );
	}

	void PerspectiveCamera::Load( Archive& archive )
	{
		Camera::Load( archive );

		archive.Read( mMatrixProjection.Ptr(), ar_sizeof( mMatrixProjection ));
		archive.Read( &mNearZ );
		archive.Read( &mFarZ );
		archive.Read( &mFOV );
	}

	//////////////////////////////

	GameComponent* PerspectiveCamera::Copy()
	{
		PerspectiveCamera* camera = new PerspectiveCamera();

		Camera::Copy( camera );

		camera->mMatrixProjection = mMatrixProjection;
		camera->mNearZ = mNearZ;
		camera->mFarZ  = mFarZ;
		camera->mFOV   = mFOV;

		return camera;
	}
}}
