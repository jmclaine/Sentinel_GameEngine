#include "EditorControllerComponent.h"
#include "Component/Physics.h"
#include "Component/Transform.h"
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
#include "Component/PerspectiveCamera.h"
#include "Component/OrthographicCamera.h"

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
		Component::Controller3D::Startup();
	}

	void EditorControllerComponent::Update()
	{
		// Adjust the camera sizes if necessary.
		//
		WindowInfo* info = mOwner->GetWorld()->mRenderer->GetWindow();

		if( mLastWindowWidth != mTransform->mScale.x || mLastWindowHeight != mTransform->mScale.y )
		{
			// Magically ensure the FOV looks correct.
			// Divide by 32 from the width produces the intended result.
			//
			mWorldCamera->Set( (UINT)mTransform->mScale.x, (UINT)mTransform->mScale.y, mWorldCamera->NearZ(), mWorldCamera->FarZ(), (float)(info->Width() >> 5) );
		
			/////////////////////////////////////

			mEditorCamera->Set( info->Width(), info->Height() );

			mLastWindowWidth  = mTransform->mScale.x;
			mLastWindowHeight = mTransform->mScale.y;
		}

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

			int scroll = mouse.ScrollDistance();

			// Forward.
			//
			if( scroll > 0 )
				impulse += mTransform->GetMatrixWorld().Forward();
		
			// Backward.
			//
			if( scroll < 0 )
				impulse -= mTransform->GetMatrixWorld().Forward();

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

				impulse -= mTransform->GetMatrixWorld().Right() * x;
				impulse += mTransform->GetMatrixWorld().Up() * y;

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

				impulse = Vector3f( 0, 0, 0 );

				if( keyboard.IsDown( 'W' ))
				{
					impulse += mTransform->GetMatrixWorld().Forward();
				}

				if( keyboard.IsDown( 'S' ))
				{
					impulse -= mTransform->GetMatrixWorld().Forward();
				}

				if( keyboard.IsDown( 'D' ))
				{
					impulse += mTransform->GetMatrixWorld().Right();
				}

				if( keyboard.IsDown( 'A' ))
				{
					impulse -= mTransform->GetMatrixWorld().Right();
				}

				if( keyboard.IsDown( 'E' ))
				{
					impulse += mTransform->GetMatrixWorld().Up();
				}

				if( keyboard.IsDown( 'Q' ))
				{
					impulse -= mTransform->GetMatrixWorld().Up();
				}
				
				// Move in direction.
				//
				if( impulse.LengthSquared() > 0 )
					body->ApplyCentralImpulse( impulse.Normalize() * mStrafeSpeed );

				didMove = true;
			}

			if( didMove )
				SetCursorPos( mLastMousePos.x, mLastMousePos.y );
		}
	}

	void EditorControllerComponent::Shutdown()
	{
		Component::Controller3D::Shutdown();
	}
}
