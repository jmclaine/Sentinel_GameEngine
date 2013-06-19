#include "Util.h"
#include "GameObject.h"
#include "CameraComponent.h"

namespace Sentinel
{
	CameraComponent::CameraComponent()
	{
		mComponentType = COMPONENT_CAMERA;

		mTransform = NULL;

		mMatrixView.Identity();
		mMatrixProjection.Identity();
		mMatrixFinal.Identity();
	}

	void CameraComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( COMPONENT_TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "CameraComponent::Startup()\n" + mOwner->mName + " does not contain TransformComponent" );
	}

	void CameraComponent::Update()
	{}

	void CameraComponent::Shutdown()
	{}

	// Create the frustum.
	//
	/*void CameraComponent::setFrustum( HWND hWnd, float nearZ, float farZ, float FOV )
	{
		static vec3f ftl, ftr, fbl, fbr,
					 ntl, ntr, nbl, nbr;

		vec3f nc, fc, right, up, dir;

		dir = facingDirection();

		right = vec3f( 0, 1, 0 ).cross( dir ).normalize();

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
		m_plane[1].normal = vec3f(-dir.x, -dir.y, -dir.z);

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
	bool CameraComponent::checkFrustum( BoundingSphere& sphere )
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
	bool CameraComponent::checkFrustum( FrustumBox& box )
	{
		float distance;

		for( int axis = 0; axis < 6; ++axis )
		{
			int counter = 8;
			for( int i = 0; i < 8; ++i )
			{
				distance = m_plane[ axis ].distance( box.pos[ i ] );

				if( distance > 0 )
				{
					return true;
				}
				else
				{
					--counter;
				}
			}

			if( counter == 0 )
			{
				return false;
			}
		}

		return false;	// should never reach here
	}*/
}
