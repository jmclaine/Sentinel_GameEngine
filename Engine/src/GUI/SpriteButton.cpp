#include "GUI/SpriteButton.h"
#include "Component/SpriteDrawable.h"
#include "GameObject.h"

namespace Sentinel {
namespace GUI
{
	DEFINE_SERIAL_REGISTER(SpriteButton);
	DEFINE_SERIAL_CLONE(SpriteButton);

	SpriteButton::SpriteButton() :
		mFrameUp(0),
		mColorUp(1, 1, 1, 1),
		mFrameOver(0),
		mColorOver(0, 0.8f, 1, 1),
		mFrameDown(0),
		mColorDown(0, 0, 0.5f, 1)
	{}

	SpriteButton::~SpriteButton()
	{}

	void SpriteButton::Startup()
	{
		SpriteController::Startup();

		mSprite = mOwner->GetComponent<Component::SpriteDrawable>();

		if (mSprite == NULL)
			throw AppException("SpriteButton::Startup()\n" + std::string(mOwner->mName) + " does not contain SpriteComponent");
	}

	void SpriteButton::Update()
	{
		SpriteController::Update();
		Button::Update(mIsOver);

		if (mEnabled)
		{
			switch (mState)
			{
			case UP:
				mSprite->mFrame = mFrameUp;
				mSprite->mColor = mColorUp;
				break;

			case OVER:
				mSprite->mFrame = mFrameOver;
				mSprite->mColor = mColorOver;
				break;

			case DOWN:
				mSprite->mFrame = mFrameDown;
				mSprite->mColor = mColorDown;
				break;
			}
		}
	}

	void SpriteButton::Shutdown()
	{
		SpriteController::Shutdown();
	}

	///////////////////////////////////

	DEFINE_SERIAL_REGISTER_SAVE(SpriteButton);

	void SpriteButton::Save(Sentinel::Archive& archive)
	{
		Button::Save(archive);

		SpriteController::Save(archive);

		archive.Write(&mFrameUp);
		archive.Write(&mFrameOver);
		archive.Write(&mFrameDown);
	}

	void SpriteButton::Load(Sentinel::Archive& archive)
	{
		Button::Load(archive);

		SpriteController::Load(archive);

		archive.Read(&mFrameUp);
		archive.Read(&mFrameOver);
		archive.Read(&mFrameDown);
	}

	///////////////////////////////////

	GameComponent* SpriteButton::Copy()
	{
		SpriteButton* button = new SpriteButton();

		SpriteController::Copy(button);

		button->mFrameUp = mFrameUp;
		button->mFrameOver = mFrameOver;
		button->mFrameDown = mFrameDown;

		return button;
	}
}}
