#include "Util.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "TransformComponent.h"

namespace Sentinel
{
	CameraComponent::CameraComponent() :
		mTransform( NULL )
	{
		mType = GameComponent::CAMERA;

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
		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "CameraComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void CameraComponent::Update()
	{}

	void CameraComponent::Shutdown()
	{
		mTransform = NULL;
	}
}
