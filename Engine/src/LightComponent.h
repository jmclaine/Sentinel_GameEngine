#pragma once

#include <vector>

#include "GameComponent.h"
#include "ColorRGBA.h"
#include "Vector4f.h"

namespace Sentinel
{
	class TransformComponent;
	class DrawableComponent;
	class Texture;
	class RenderTexture;

	class SENTINEL_DLL LightComponent : public GameComponent
	{
	protected:

		virtual DECLARE_SERIAL_SAVE();
		virtual DECLARE_SERIAL_LOAD();

		TransformComponent*		mTransform;

		std::vector< DrawableComponent* > mDynamic;

		Texture*				mTexture;
		RenderTexture*			mRenderTexture;
		

	public:

		ColorRGBA				mColor;
		Vector4f				mAttenuation;	// radius = w
		
	protected:

		LightComponent();

	public:

		virtual ~LightComponent();

		virtual void		Startup();

		virtual void		Update();

		virtual void		Shutdown();

		///////////////////////////////////////

		TransformComponent* GetTransform();

		Texture*			GetTexture();
		RenderTexture*		GetRenderTexture();
	};
}
