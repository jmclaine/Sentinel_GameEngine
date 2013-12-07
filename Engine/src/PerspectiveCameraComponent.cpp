#include "PerspectiveCameraComponent.h"
#include "TransformComponent.h"
#include "Archive.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameWorld.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( PerspectiveCameraComponent );
	DEFINE_SERIAL_CLONE( PerspectiveCameraComponent );

	PerspectiveCameraComponent::PerspectiveCameraComponent() :
		mNearZ( 0.1f ),
		mFarZ( 10000.0f ),
		mFOV( 60.0f )
	{
		Set( 1920, 1080 );
	}

	PerspectiveCameraComponent::PerspectiveCameraComponent( float windowWidth, float windowHeight ) :
		mNearZ( 0.1f ),
		mFarZ( 10000.0f ),
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

	const Vector3f& PerspectiveCameraComponent::LookAt()
	{
		return mLookAt;
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

	// Create the frustum.
	//
	/*void PerspectiveCameraComponent::CreateFrustum()
	{
		static Vector3f ftl, ftr, fbl, fbr,
					 ntl, ntr, nbl, nbr;

		Vector3f nc, fc, right, up, dir;

		dir = facingDirection();

		right = Vector3f( 0, 1, 0 ).cross( dir ).normalize();

		up = dir.cross( right );

		nc = m_position + dir * nearZ;
		fc = m_position + dir * (farZ * 0.5f);

		// Calculate aspect ratio.
		//
		RECT clientRect;
		GetClientRect( hWnd, &clientRect );
		float aspectRatio = (clientRect.right - clientRect.left) / static_cast< float >( clientRect.bottom - clientRect.top );

		// Setup camera frustum width and height of near and far planes.
		//
		float angle = tan( (float)DEGREES_TO_RADIANS * FOV );

		float nearHeight = nearZ * angle * 0.5f;
		float nearWidth  = nearHeight * aspectRatio * 0.5f;
		float farHeight  = farZ * angle * 0.5f;
		float farWidth   = farHeight * aspectRatio * 0.5f;

		ftl = fc + (up * farHeight) + (right * farWidth);
		ftr = fc + (up * farHeight) - (right * farWidth);
		fbl = fc - (up * farHeight) + (right * farWidth);
		fbr = fc - (up * farHeight) - (right * farWidth);

		ntl = nc + (up * nearHeight) + (right * nearWidth);
		ntr = nc + (up * nearHeight) - (right * nearWidth);
		nbl = nc - (up * nearHeight) + (right * nearWidth);
		nbr = nc - (up * nearHeight) - (right * nearWidth);

		//NEAR
		m_plane[0].pos = nc;
		m_plane[0].normal = dir;

		//FAR
		m_plane[1].pos = fc;
		m_plane[1].normal = Vector3f(-dir.X(), -dir.Y(), -dir.Z());

		//TOP
		m_plane[2].pos = ntl;
		m_plane[2].normal = (ntl-ftl).cross(ntl-ntr).normalize();

		//BOTTOM
		m_plane[3].pos = nbl;
		m_plane[3].normal = (nbl-nbr).cross(nbl-fbl).normalize();

		//LEFT
		m_plane[4].pos = ntl;
		m_plane[4].normal = (ntl-nbl).cross(ntl-ftl).normalize();

		//RIGHT
		m_plane[5].pos = ntr;
		m_plane[5].normal = (nbr-ntr).cross(nbr-fbr).normalize();
	}

	// Check for a sphere being within the frustum.
	//
	bool PerspectiveCameraComponent::CheckFrustum( BoundingSphere& sphere )
	{
		float distance;

		for( int axis = 0; axis < 6; ++axis )
		{
			distance = m_plane[ axis ].distance( sphere.center );

			if( distance < -sphere.radius )
			{
				return false;
			}
		}
		return true;
	}

	// Check for a box being within the frustum.
	//
	bool PerspectiveCameraComponent::CheckFrustum( const FrustumBox& box )
	{
		float distance;
		int   counter;

		for( int axis = 0; axis < 6; ++axis )
		{
			counter = 8;
			for( int i = 0; i < 8; ++i )
			{
				distance = m_plane[ axis ].distance( box.pos[ i ] );

				if( distance > 0 )
					return true;
				else
					--counter;
			}

			if( counter == 0 )
				return false;
		}

		return false;	// should never reach here
	}
	//*/
}
