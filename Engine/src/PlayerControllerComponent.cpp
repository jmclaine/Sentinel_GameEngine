#include "Util.h"
#include "Input.h"
#include "Timing.h"
#include "PlayerControllerComponent.h"

namespace Sentinel
{
	PlayerControllerComponent::PlayerControllerComponent()
	{
		mSpeed			= 1.0f;
		mAngularSpeed	= 0.2f;
	}

	void PlayerControllerComponent::Update()
	{
		Keyboard* keyboard = Keyboard::Inst();

		vec3f impulse( 0, 0, 0 );

		mPhysics->mRigidBody->activate( true );

		mat4f matRot( mTransform->mOrientation.Matrix() );
		
		// Forward.
		//
		if( keyboard->IsDown( 'W' ))
			impulse += matRot.Forward();
		
		// Backward.
		//
		if( keyboard->IsDown( 'S' ))
			impulse += -matRot.Forward();
		
		// Left.
		//
		if( keyboard->IsDown( 'A' ))
			impulse += -matRot.Right();
		
		// Right.
		//
		if( keyboard->IsDown( 'D' ))
			impulse += matRot.Right();
		
		// Up.
		//
		if( keyboard->IsDown( VK_SPACE ))
			impulse += matRot.Up();
		
		// Down.
		//
		if( keyboard->IsDown( 'C' ))
			impulse += -matRot.Up();

		// Move in direction.
		//
		if( impulse.LengthSquared() > 0 )
			mPhysics->mRigidBody->applyCentralImpulse( btVector3( impulse.x, impulse.y, impulse.z ).normalize() * mSpeed );

		///////////////////////////////////

		Mouse* mouse = Mouse::Inst();
		POINT  mousePos;

		GetCursorPos( &mousePos );
		vec2i center = CenterHandle( Mouse::mHWND );
		vec3f diff = vec3f( (float)(center.x-mousePos.x), (float)(center.y-mousePos.y), 0 ) * mAngularSpeed;

		if( keyboard->IsDown( VK_UP ))
			diff.z += 1;

		if( keyboard->IsDown( VK_DOWN ))
			diff.z -= 1;

		SetCursorPos( center.x, center.y );

		// Rotate in direction with spherical interpolation.
		//
		btTransform transform = mPhysics->mRigidBody->getWorldTransform();

		static btQuaternion qFinal = transform.getRotation();
		
		if( diff.LengthSquared() > 0 )
		{
			static vec3f rot;
			rot += diff * (float)DEGREES_TO_RADIANS;
	
			qFinal = btQuaternion( rot.x, rot.y, rot.z );
		}

		btQuaternion qResult = slerp( transform.getRotation(), qFinal, clamp( Timing::Inst()->DeltaTime()*10.0f, 0.0f, 1.0f ));

		if( qResult.length2() > 0 )	// strangely, slerp can end with an invalid rotation
			transform.setRotation( qResult );
		
		mPhysics->mRigidBody->setWorldTransform( transform );
	}
}