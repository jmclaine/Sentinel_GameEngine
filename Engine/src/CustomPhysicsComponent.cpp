#include "CustomPhysicsComponent.h"
#include "GameObject.h"

namespace Sentinel
{
	CustomPhysicsComponent::CustomPhysicsComponent()
	{
#ifdef IGNORE_PHYSICS
		mIgnoreRB			= NULL;
#endif

		mShapeType			= SHAPE_INVALID;
		mMass				= 1.0f;

		mShapeScale			= Vector3f( 1, 1, 1 );

		mFlags				= 0;
		mLinearDamping		= 0;
		mAngularDamping		= 0;
		mRestitution		= 0;
		mFriction			= 0.5f;
	}

	CustomPhysicsComponent::~CustomPhysicsComponent()
	{
		mTransform = NULL;
	}

	void CustomPhysicsComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "PhysicsComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void CustomPhysicsComponent::Update()
	{}
		
	void CustomPhysicsComponent::Shutdown()
	{
		mTransform = NULL;
	}

	/////////////////////////////////

	void CustomPhysicsComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		int type = (int)mShapeType;
		
		if( type == SHAPE_INVALID )
			REPORT_ERROR( "No support for PhysicsComponent shape on " << mOwner->mName, "Save Error" );

		archive.Write( &type );

		archive.Write( &mMass );

		archive.Write( mShapePosition.Ptr(),	ar_sizeof( mShapePosition ));
		archive.Write( mShapeOrientation.Ptr(), ar_sizeof( mShapeOrientation ));
		archive.Write( mShapeScale.Ptr(),		ar_sizeof( mShapeScale ));

		archive.Write( &mFlags );
		archive.Write( &mLinearDamping );
		archive.Write( &mAngularDamping );
		archive.Write( &mRestitution );
		archive.Write( &mFriction );

		archive.Write( mAngularFactor.Ptr(),	ar_sizeof( mAngularFactor ));
		archive.Write( mGravity.Ptr(),			ar_sizeof( mGravity ));
	}

	void CustomPhysicsComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		int type = (int)mShapeType;
		
		if( type == SHAPE_INVALID )
			REPORT_ERROR( "No support for PhysicsComponent shape on " << mOwner->mName, "Save Error" );

		archive.Read( &type );

		archive.Read( &mMass );

		archive.Read( mShapePosition.Ptr(),		ar_sizeof( mShapePosition ));
		archive.Read( mShapeOrientation.Ptr(),	ar_sizeof( mShapeOrientation ));
		archive.Read( mShapeScale.Ptr(),		ar_sizeof( mShapeScale ));

		archive.Read( &mFlags );
		archive.Read( &mLinearDamping );
		archive.Read( &mAngularDamping );
		archive.Read( &mRestitution );
		archive.Read( &mFriction );

		archive.Read( mAngularFactor.Ptr(),		ar_sizeof( mAngularFactor ));
		archive.Read( mGravity.Ptr(),			ar_sizeof( mGravity ));
	}
}
