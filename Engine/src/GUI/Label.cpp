#include "GUI/Label.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "FontSystem.h"
#include "Font.h"
#include "SpriteSystem.h"
#include "RenderManager.h"
#include "Material.h"
#include "Component/Transform.h"

namespace Sentinel
{
	namespace GUI
	{
		SerialRegister Label::SERIAL_REGISTER("Label", Label::Clone);
		Serializable* Label::Clone() { return new Label(); }

		Label::Label()
		{ }

		Label::Label(const std::string& text) :
			mText(text),
			mColor(1, 1, 1, 1)
		{ }

		Label::~Label()
		{ }

		//////////////////////////////////

		void Label::Startup()
		{
			Component::Drawable::Startup();

			auto world = mOwner->GetWorld();
			auto font = mFont.lock();
			world->mRenderManager->Add(this, font->mMaterial->mRenderQueue);
		}

		void Label::Update()
		{ }

		void Label::Shutdown()
		{
			auto world = mOwner->GetWorld();
			auto font = mFont.lock();
			world->mRenderManager->Remove(this, font->mMaterial->mRenderQueue);
		}

		//////////////////////////////////

		void Label::CalculateBounds()
		{ }

		bool Label::CheckVisible(Component::Camera* camera)
		{
			return true;
		}

		void Label::Draw(Component::Camera* camera)
		{
			if (mEnabled)
			{
				auto world = mOwner->GetWorld();
				world->mFontSystem->Clear();

				const auto font = mFont.lock();
				
				Vector2 scale(mTransform->mScale.x / font->mSize.x, mTransform->mScale.y / font->mSize.y);

				Matrix4x4 matWVP = camera->GetMatrixWVP();

				Vector3 offset;

				for (UINT i = 0; i < mText.size(); ++i)
				{
					char& c = mText[i];

					offset.x += font->mOffsetX[c] * scale.x;
					offset.y = -font->mOffsetY[c] * scale.y;

					world->mFontSystem->Draw(font.get(), c, mColor, matWVP * mTransform->GetMatrixWorld(offset));

					offset.x += font->mAdvance[c] * scale.x;
					offset.z -= 0.0001f;
				}

				world->mFontSystem->Present(camera);
			}
		}

		//////////////////////////////////

		void Label::SerialSave(Archive& archive)
		{
			SERIAL_REGISTER.Save(archive);
			Save(archive);
		}

		void Label::Save(Archive& archive)
		{
			Component::Drawable::Save(archive);

			archive.Write(&mText);
			archive.Write(&mColor);
		}

		void Label::Load(Archive& archive)
		{
			Component::Drawable::Load(archive);

			archive.Read(&mText);
			archive.Read(&mColor);
		}

		//////////////////////////////////

		GameComponent* Label::Copy()
		{
			Label* label = new Label(mText);

			Component::Drawable::Copy(label);

			label->mColor = mColor;

			return label;
		}
	}
}
