#pragma once

#include "GUI/SpriteController.h"
#include "Matrix4x4.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "Input.h"

namespace Sentinel {
namespace GUI
{
	DEFINE_SERIAL_REGISTER(SpriteController);
	DEFINE_SERIAL_CLONE(SpriteController);

	SpriteController::SpriteController() :
		mPositionToWindowX(false),
		mPositionToWindowY(false),
		mScaleToWindowX(false),
		mScaleToWindowY(false)
	{}

	SpriteController::~SpriteController()
	{}

	///////////////////////////////////

	void SpriteController::Startup()
	{
		Controller2D::Startup();

		mBasePosition = mTransform->mPosition;
		mBaseScale = mTransform->mScale;
	}

	void SpriteController::Update()
	{
		WindowInfo* info = mOwner->GetWorld()->mRenderer->GetWindow();

		// Adjust position.
		//
		Vector3* pos = &mTransform->mPosition;

		*pos = mBasePosition;

		if (mPositionToWindowX)
			pos->x *= info->WidthRatio();

		if (mPositionToWindowY)
			pos->y *= info->HeightRatio();

		pos->x += mMargin.left;
		pos->y += mMargin.top;

		// Adjust scale.
		//
		Vector3* scale = &mTransform->mScale;

		*scale = mBaseScale;

		if (mScaleToWindowX)
			scale->x *= info->WidthRatio();

		if (mScaleToWindowY)
			scale->y *= info->HeightRatio();

		scale->x += mMargin.right;
		scale->y += mMargin.bottom;

		// Check for mouse being over the widget.
		//
		POINT mousePos = Mouse::Get().GetPosition((HWND)info->Handle());

		Vector3 v = mTransform->GetMatrixWorld().Inverse().Transform(Vector3((float)mousePos.x, (float)mousePos.y, 0));

		if (v.x >= 0 && v.y >= 0 &&
			v.x <= 1 && v.y <= 1)
		{
			if (!mIsOver)
			{
				mIsOver = true;
				mActionEnter();
			}
			else
			{
				mActionOver();
			}
		}
		else if (mIsOver)
		{
			mIsOver = false;
			mActionExit();
		}
	}

	void SpriteController::Shutdown()
	{
		mTransform->mPosition = mBasePosition;
		mTransform->mScale = mBaseScale;
	}

	DEFINE_SERIAL_REGISTER_SAVE(SpriteController);

	void SpriteController::Save(Archive& archive)
	{
		Controller2D::Save(archive);

		archive.Write(&mPositionToWindowX);
		archive.Write(&mPositionToWindowY);

		archive.Write(&mScaleToWindowX);
		archive.Write(&mScaleToWindowY);

		archive.Write(mMargin.Ptr(), ar_sizeof(mMargin));
	}

	void SpriteController::Load(Archive& archive)
	{
		Controller2D::Load(archive);

		archive.Read(&mPositionToWindowX);
		archive.Read(&mPositionToWindowY);

		archive.Read(&mScaleToWindowX);
		archive.Read(&mScaleToWindowY);

		archive.Read(mMargin.Ptr(), ar_sizeof(mMargin));
	}

	///////////////////////////////////

	GameComponent* SpriteController::Copy()
	{
		SpriteController* controller = new SpriteController();

		Controller2D::Copy(controller);

		controller->mPositionToWindowX = mPositionToWindowX;
		controller->mPositionToWindowY = mPositionToWindowY;

		controller->mScaleToWindowX = mScaleToWindowX;
		controller->mScaleToWindowY = mScaleToWindowY;

		return controller;
	}

	void SpriteController::Copy(GameComponent* component)
	{
		Controller2D::Copy(component);
	}
}}
