#pragma once

#include "GUI\Controller2D.h"
#include "GameObject.h"

namespace Sentinel { namespace GUI
{
	Controller2D::Controller2D() :
		mIsOver( false )
	{
		mType = GameComponent::CONTROLLER;
	}

	Controller2D::~Controller2D()
	{}

	void Controller2D::Startup()
	{
		mTransform = (Component::Transform*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "ControllerComponent::Startup()\n" + mOwner->mName + " does not contain TransformComponent" );
	}

	void Controller2D::Save( Archive& archive )
	{}

	void Controller2D::Load( Archive& archive )
	{}

	///////////////////////////////

	void Controller2D::Copy( GameComponent* component )
	{
		GameComponent::Copy( component );
	}
}}
