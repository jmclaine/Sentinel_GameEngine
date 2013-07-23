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

	const TransformComponent* CameraComponent::GetTransform()
	{
		return mTransform;
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
}
