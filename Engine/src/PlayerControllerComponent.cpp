#include "MathCommon.h"
#include "Util.h"
#include "Input.h"
#include "Timing.h"
#include "PlayerControllerComponent.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( PlayerControllerComponent );
	DEFINE_SERIAL_CLONE( PlayerControllerComponent );

	PlayerControllerComponent::PlayerControllerComponent()
	{
		mSpeed			= 1.0f;
		mAngularSpeed	= 0.2f;
	}

	void PlayerControllerComponent::Startup()
	{
		ControllerComponent::Startup();
	}

	void PlayerControllerComponent::Update()
	{
		ControllerComponent::Update();

		if( mPhysics )
		{
			Keyboard* keyboard = Keyboard::Inst();

			Vector3f impulse( 0, 0, 0 );

			mPhysics->GetRigidBody()->activate( true );

			Matrix4f matRot( mTransform->mOrientation );
		
			// Forward.
			//
			if( keyboard->IsDown( 'W' ))
				impulse = impulse.Add( matRot.Forward() );
		
			// Backward.
			//
			if( keyboard->IsDown( 'S' ))
				impulse = impulse.Add( matRot.Forward() * -1.0f );
		
			// Left.
			//
			if( keyboard->IsDown( 'A' ))
				impulse = impulse.Add( matRot.Right() * -1.0f );
		
			// Right.
			//
			if( keyboard->IsDown( 'D' ))
				impulse = impulse.Add( matRot.Right() );
		
			// Up.
			//
			if( keyboard->IsDown( VK_SPACE ))
				impulse = impulse.Add( matRot.Up() );
		
			// Down.
			//
			if( keyboard->IsDown( 'C' ))
				impulse = impulse.Add( matRot.Up() * -1.0f );

			// Move in direction.
			//
			if( impulse.LengthSquared() > 0 )
				mPhysics->GetRigidBody()->applyCentralImpulse( btVector3( impulse.x, impulse.y, impulse.z ).normalize() * mSpeed );
		
			///////////////////////////////////

			Mouse* mouse = Mouse::Inst();
			POINT  mousePos;

			GetCursorPos( &mousePos );
			POINT center = CenterHandle();
			Vector3f diff = Vector3f( (float)(center.x-mousePos.x), (float)(center.y-mousePos.y), 0 ) * mAngularSpeed;

			if( keyboard->IsDown( VK_UP ))
				diff.z += 1.0f;

			if( keyboard->IsDown( VK_DOWN ))
				diff.z -= 1.0f;

			SetCursorPos( center.x, center.y );

			// Rotate in direction with spherical interpolation.
			//
			btTransform transform = mPhysics->GetRigidBody()->getWorldTransform();

			static btQuaternion qFinal = transform.getRotation();
		
			if( diff.LengthSquared() > 0 )
			{
				static Vector3f rot;
				rot = rot + diff * (float)DEGREES_TO_RADIANS;
	
				qFinal = btQuaternion( rot.x, rot.y, rot.z );
			}

			btQuaternion qResult = slerp( transform.getRotation(), qFinal, clamp( Timing::Inst()->DeltaTime()*10.0f, 0.0f, 1.0f ));

			if( qResult.length2() > 0 )	// strangely, slerp can end with an invalid rotation
				transform.setRotation( qResult );
		
			mPhysics->GetRigidBody()->setWorldTransform( transform );
		}
	}

	void PlayerControllerComponent::Shutdown()
	{
		ControllerComponent::Shutdown();
	}

	/////////////////////////////////

	void PlayerControllerComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		archive.Write( &mSpeed );
		archive.Write( &mAngularSpeed );
	}

	void PlayerControllerComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( &mSpeed );
		archive.Read( &mAngularSpeed );
	}
}