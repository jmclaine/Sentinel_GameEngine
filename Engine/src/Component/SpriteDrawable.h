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
		DECLARE_SERIAL();

	protected:

		SpriteSystem* mSpriteSystem;

	public:

		UINT mFrame;
		ColorRGBA mColor;

		std::shared_ptr<Sprite> mSprite;

	protected:

		SpriteDrawable();

	public:

		SpriteDrawable(std::shared_ptr<Sprite> sprite);
		~SpriteDrawable();

		void Set(std::shared_ptr<Sprite> sprite);

		/////////////////////////////////

		void Startup();

		/////////////////////////////////

		bool CheckVisible(Camera* camera);

		void CalculateBounds();

		void Draw();

		///////////////////////////////////

		GameComponent* Copy();
	};
}}
