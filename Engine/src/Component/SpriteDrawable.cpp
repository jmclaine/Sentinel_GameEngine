#include "Component/SpriteDrawable.h"
#include "SpriteManager.h"
#include "SpriteSystem.h"
#include "Vector2f.h"
#include "Matrix4f.h"
#include "Material.h"
#include "Sprite.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Mesh.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Archive.h"
#include "RenderManager.h"

namespace Sentinel {
namespace Component
{
	DEFINE_SERIAL_REGISTER(SpriteDrawable);
	DEFINE_SERIAL_CLONE(SpriteDrawable);

	SpriteDrawable::SpriteDrawable() :
		mSpriteSystem(NULL),
		mFrame(0)
	{}

	SpriteDrawable::SpriteDrawable(std::shared_ptr<Sprite> sprite) :
		mSpriteSystem(NULL),
		mFrame(0)
	{
		Set(sprite);
	}

	SpriteDrawable::~SpriteDrawable()
	{}

	void SpriteDrawable::Set(std::shared_ptr<Sprite> sprite)
	{
		_ASSERT(sprite);

		mSprite = sprite;
	}

	/////////////////////////////////

	void SpriteDrawable::Startup()
	{
		Drawable::Startup();

		mSpriteSystem = mOwner->GetWorld()->mSpriteSystem;

		if (!mSpriteSystem)
			throw AppException("SpriteDrawable::Startup()\n" + std::string(mOwner->mName) + " does not contain SpriteSystem");

		mOwner->GetWorld()->mRenderManager->Draw(this, RenderQueue::ALPHA_BLEND, mOwner->mLayer);
	}

	/////////////////////////////////

	void SpriteDrawable::CalculateBounds()
	{}

	bool SpriteDrawable::CheckVisible(Camera* camera)
	{
		return true;
	}

	void SpriteDrawable::Draw()
	{
		Camera* camera = mOwner->GetWorld()->mCurrentCamera;

		mSpriteSystem->mCamera = camera;
		mSpriteSystem->mSprite = mSprite;

		mSpriteSystem->Clear();

		mSpriteSystem->Draw(mFrame, mColor, camera->GetMatrixWVP() * mTransform->GetMatrixWorld());

		mSpriteSystem->Present();
	}

	/////////////////////////////////

	void SpriteDrawable::Save(Archive& archive)
	{
		_ASSERT(mOwner);
		_ASSERT(mOwner->GetWorld());
		_ASSERT(mOwner->GetWorld()->mSpriteManager);

		SERIAL_REGISTER.Save(archive);

		Drawable::Save(archive);

		archive.Write(&mOwner->GetWorld()->mSpriteManager->Get(mSprite));
		archive.Write(&mFrame);
		archive.Write(mColor.Ptr(), ar_sizeof(mColor));
	}

	void SpriteDrawable::Load(Archive& archive)
	{
		_ASSERT(mOwner);
		_ASSERT(mOwner->GetWorld());
		_ASSERT(mOwner->GetWorld()->mSpriteManager);

		Drawable::Load(archive);

		std::string name;
		archive.Read(&name);

		mSprite = mOwner->GetWorld()->mSpriteManager->Get(name);

		archive.Read(&mFrame);
		archive.Read(mColor.Ptr(), ar_sizeof(mColor));
	}

	///////////////////////////////////

	GameComponent* SpriteDrawable::Copy()
	{
		SpriteDrawable* drawable = new SpriteDrawable(mSprite);

		Drawable::Copy(drawable);

		drawable->mFrame = mFrame;
		drawable->mColor = mColor;

		return drawable;
	}
}}
