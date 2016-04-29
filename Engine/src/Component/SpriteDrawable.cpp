#include "Component/SpriteDrawable.h"
#include "SpriteManager.h"
#include "SpriteSystem.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "Sprite.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Mesh.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Archive.h"
#include "RenderManager.h"
#include "Exception.h"

namespace Sentinel
{
	namespace Component
	{
		SerialRegister SpriteDrawable::SERIAL_REGISTER("SpriteDrawable", SpriteDrawable::Clone);
		Serializable* SpriteDrawable::Clone() { return new SpriteDrawable(); }

		SpriteDrawable::SpriteDrawable() :
			mFrame(0)
		{ }

		SpriteDrawable::SpriteDrawable(std::weak_ptr<Sprite>& sprite) :
			mFrame(0)
		{
			mSprite = sprite;
		}

		SpriteDrawable::SpriteDrawable(std::weak_ptr<Sprite>&& sprite) :
			mFrame(0)
		{
			mSprite = sprite;
		}

		SpriteDrawable::~SpriteDrawable()
		{ }

		/////////////////////////////////

		void SpriteDrawable::Startup()
		{
			_ASSERT(mSprite.lock() != nullptr);

			Drawable::Startup();

			auto world = mOwner->GetWorld();
			world->mRenderManager->Add(this, world->mSpriteSystem->mMaterial.lock()->mRenderQueue);
		}

		void SpriteDrawable::Shutdown()
		{
			auto world = mOwner->GetWorld();
			world->mRenderManager->Remove(this, world->mSpriteSystem->mMaterial.lock()->mRenderQueue);
		}

		/////////////////////////////////

		void SpriteDrawable::Draw(Camera* camera)
		{
			auto world = mOwner->GetWorld();

			world->mSpriteSystem->mSprite = mSprite;
			world->mSpriteSystem->Clear();
			world->mSpriteSystem->Draw(mFrame, mColor, camera->GetMatrixWVP() * mTransform->GetMatrixWorld());
			world->mSpriteSystem->Present(camera);
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
			archive.Write(&mColor);
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
			archive.Read(&mColor);
		}

		///////////////////////////////////

		GameComponent* SpriteDrawable::Copy()
		{
			SpriteDrawable* drawable = new SpriteDrawable(mSprite);

			Copy(drawable);
			
			return drawable;
		}

		void SpriteDrawable::Copy(GameComponent* component)
		{
			Drawable::Copy(component);

			auto drawable = static_cast<SpriteDrawable*>(component);

			drawable->mFrame = mFrame;
			drawable->mColor = mColor;
		}
	}
}
