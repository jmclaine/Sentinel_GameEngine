#include "Component/Controller3D.h"
#include "GameObject.h"
#include "Component/Physics.h"
#include "Component/Transform.h"

namespace Sentinel { namespace Component
{
	Controller3D::Controller3D()
	{
		mType = GameComponent::CONTROLLER;

		Shutdown();
	}

	void Controller3D::Startup()
	{
		mPhysics = (Physics*)mOwner->FindComponent( GameComponent::PHYSICS );

		if( mPhysics == NULL )
			throw AppException( "Controller::Startup()\n" + std::string( mOwner->mName ) + " does not contain Physics" );

		///////////////////////////

		mTransform = (Transform*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "Controller::Startup()\n" + std::string( mOwner->mName ) + " does not contain Transform" );
	}

	void Controller3D::Update()
	{}

	void Controller3D::Shutdown()
	{
		mPhysics   = NULL;
		mTransform = NULL;
	}
}}
