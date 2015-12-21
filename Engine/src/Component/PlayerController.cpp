#include "Component/PlayerController.h"
#include "Component/Physics.h"
#include "Component/Transform.h"
#include "MathUtil.h"
#include "Input.h"
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

namespace Sentinel {
namespace Component
{
	DEFINE_SERIAL_REGISTER(PlayerController);
	DEFINE_SERIAL_CLONE(PlayerController);

	PlayerController::PlayerController()
	{
		mSpeed = 1.0f;
		mAngularSpeed = 0.2f;
	}

	void PlayerController::Startup()
	{
		Controller3D::Startup();
	}

	void PlayerController::Shutdown()
	{
		Controller3D::Shutdown();
	}

	/////////////////////////////////

	void PlayerController::Execute()
	{
		if (mPhysics)
		{
			Keyboard& keyboard = Keyboard::Get();

			Vector3f impulse(0, 0, 0);

			// Should have the keys mapped to an input handler.

			// Forward.
			//
			if (keyboard.IsDown('W'))
				impulse += mTransform->mOrientation.Forward();

			// Backward.
			//
			if (keyboard.IsDown('S'))
				impulse -= mTransform->mOrientation.Forward();

			// Left.
			//
			if (keyboard.IsDown('A'))
				impulse -= mTransform->mOrientation.Right();

			// Right.
			//
			if (keyboard.IsDown('D'))
				impulse += mTransform->mOrientation.Right();

			// Up.
			//
			if (keyboard.IsDown('E'))
				impulse += mTransform->mOrientation.Up();

			// Down.
			//
			if (keyboard.IsDown('Q'))
				impulse -= mTransform->mOrientation.Up();

			// Move in direction.
			//
			RigidBody* body = mPhysics->GetRigidBody();

			if (impulse.LengthSquared() > 0)
				body->ApplyCentralImpulse(impulse.Normalize() * mSpeed);

			///////////////////////////////////

			POINT mousePos = Mouse::Get().GetPosition();

			POINT center = CenterHandle((HWND)(mOwner->GetWorld()->mRenderer->GetWindow()->Handle()));
			Vector3f diff = Vector3f((float)(center.y - mousePos.y), (float)(center.x - mousePos.x), 0) * mAngularSpeed;

			if (keyboard.IsDown(VK_UP))
				diff.z += 1.0f;

			if (keyboard.IsDown(VK_DOWN))
				diff.z -= 1.0f;

			SetCursorPos(center.x, center.y);

			// Rotate in direction with spherical interpolation.
			//
			static Quatf qFinal = body->GetOrientation();

			if (diff.LengthSquared() > 0)
			{
				static Vector3f rot = qFinal.ToEuler();
				rot += diff;

				qFinal = Quatf(rot);
			}

			Quatf qResult = body->GetOrientation().Slerp(qFinal, CLAMP(mOwner->GetWorld()->mTiming->DeltaTime()*10.0f, 0.0f, 1.0f));

			if (qResult.LengthSquared() > 0)	// slerp can end with an invalid rotation
				body->SetOrientation(qResult);
		}
	}

	void PlayerController::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		GameComponent::Save(archive);

		archive.Write(&mSpeed);
		archive.Write(&mAngularSpeed);
	}

	void PlayerController::Load(Archive& archive)
	{
		GameComponent::Load(archive);

		archive.Read(&mSpeed);
		archive.Read(&mAngularSpeed);
	}

	/////////////////////////////////

	GameComponent* PlayerController::Copy()
	{
		PlayerController* controller = new PlayerController();

		GameComponent::Copy(controller);

		controller->mSpeed = mSpeed;
		controller->mAngularSpeed = mAngularSpeed;

		return controller;
	}
}}
