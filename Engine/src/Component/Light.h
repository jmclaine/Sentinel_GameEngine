#pragma once

#include <vector>

#include "GameComponent.h"
#include "ColorRGBA.h"
#include "Vector4f.h"

namespace Sentinel
{
	class Texture;
	class RenderTexture;

namespace Component
{
	class Transform;
	class Drawable;

	class SENTINEL_DLL Light : public GameComponent
	{
	protected:

		virtual DECLARE_SERIAL_SAVE();
		virtual DECLARE_SERIAL_LOAD();

		Transform* mTransform;

		std::vector<Drawable*> mDynamic;

		Texture* mTexture;
		RenderTexture* mRenderTexture;

	public:

		ColorRGBA mColor;
		Vector4f mAttenuation; // radius = w

	protected:

		Light();

	public:

		virtual ~Light();

		virtual void Startup();
		virtual void Shutdown();

		///////////////////////////////////////

		virtual void Present() = 0;

		Transform* GetTransform();

		Texture* GetTexture();
		RenderTexture* GetRenderTexture();

	protected:

		void Copy(GameComponent* component);
	};
}}
