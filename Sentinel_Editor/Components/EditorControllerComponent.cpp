#include "EditorControllerComponent.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "MathCommon.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Timing.h"
#include "Vector3f.h"
#include "Quatf.h"
#include "Matrix4f.h"
#include "Archive.h"
#include "PhysicsSystem.h"
#include "Renderer.h"
#include "GameWindow.h"

namespace Sentinel
{
	EditorControllerComponent::EditorControllerComponent()
	{
		mForwardSpeed	= 10.0f;
		mStrafeSpeed	= 3.0f;
		mAngularSpeed	= 0.2f;
	}

	void EditorControllerComponent::Startup()
	{
		ControllerComponent::Startup();
	}

	void EditorControllerComponent::Update()
	{
		if( mPhysics )
		{
			RigidBody* body		= mPhysics->GetRigidBody();

			Keyboard& keyboard	= Keyboard::Get();

			Mouse& mouse		= Mouse::Get();
			POINT mousePos		= mouse.GetPosition();

			bool didMove = false;

			if( mouse.DidGoDown( BUTTON_MIDDLE ) || mouse.DidGoDown( BUTTON_RIGHT ))
			{
				mLastMousePos = mouse.GetPosition();
			}

			Vector3f impulse( 0, 0, 0 );

			Matrix4f matRot( mTransform->mOrientation );

			int scroll = mouse.ScrollDistance();

			// Forward.
			//
			if( scroll > 0 )
				impulse += matRot.Forward();
		
			// Backward.
			//
			if( scroll < 0 )
				impulse -= matRot.Forward();

			// Move in direction.
			//
			if( impulse.LengthSquared() > 0 )
				body->ApplyCentralImpulse( impulse.Normalize() * mForwardSpeed );

			// Strafe.
			//
			if( mouse.IsDown( BUTTON_MIDDLE ))
			{
				impulse = Vector3f( 0, 0, 0 );

				float x = (float)(mLastMousePos.x-mousePos.x);
				float y = (float)(mLastMousePos.y-mousePos.y);

				impulse -= matRot.Right() * x;
				impulse += matRot.Up() * y;

				// Move in direction.
				//
				if( impulse.LengthSquared() > 0 )
					body->ApplyCentralImpulse( impulse.Normalize() * mStrafeSpeed );

				didMove = true;
			}
		
			///////////////////////////////////

			if( mouse.IsDown( BUTTON_RIGHT ))
			{
				Vector3f diff = Vector3f( (float)(mLastMousePos.y-mousePos.y), (float)(mLastMousePos.x-mousePos.x), 0 ) * mAngularSpeed;

				// Rotate in direction with spherical interpolation.
				//
				static Quatf qFinal = body->GetOrientation();
		
				if( diff.LengthSquared() > 0 )
				{
					static Vector3f rot = qFinal.ToEuler();
					rot += diff;
	
					qFinal = Quatf( rot );
				}

				Quatf qResult = body->GetOrientation().Slerp( qFinal, clamp( mOwner->GetWorld()->mTiming->DeltaTime()*10.0f, 0.0f, 1.0f ));

				if( qResult.LengthSquared() > 0 )	// slerp can end with an invalid rotation
					body->SetOrientation( qResult );

				didMove = true;
			}

			if( didMove )
				SetCursorPos( mLastMousePos.x, mLastMousePos.y );
		}
	}

	void EditorControllerComponent::Shutdown()
	{
		ControllerComponent::Shutdown();
	}
}
