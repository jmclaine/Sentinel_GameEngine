#pragma once
/*
Render a single quad sprite.
*/
#include <memory>

#include "ColorRGBA.h"
#include "Component/Drawable.h"

namespace Sentinel
{
	class SpriteSystem;
	class Sprite;

	namespace Component
	{
		class Transform;
		class Camera;

		class SENTINEL_DLL SpriteDrawable : public Drawable
		{
			static SerialRegister SERIAL_REGISTER;
			static Serializable* Clone();

		public:
			UINT mFrame;
			ColorRGBA mColor;

			std::weak_ptr<Sprite> mSprite;

		public:
			SpriteDrawable();
			SpriteDrawable(std::weak_ptr<Sprite>& sprite);
			SpriteDrawable(std::weak_ptr<Sprite>&& sprite);
			~SpriteDrawable();

			/////////////////////////////////

			void Startup();
			void Shutdown();

			/////////////////////////////////

			void Draw(Camera* camera);

			///////////////////////////////////

			void Save(Archive& archive);
			void Load(Archive& archive);

			///////////////////////////////////

			GameComponent* Copy();

		protected:
			void Copy(GameComponent* component);
		};
	}
}
