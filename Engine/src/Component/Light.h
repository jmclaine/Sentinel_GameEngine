#pragma once

#include <vector>

#include "GameComponent.h"
#include "ColorRGBA.h"
#include "Vector4.h"

namespace Sentinel
{
	class Texture;
	class RenderTexture;

namespace Component
{
	class Transform;
	class Drawable;
	class Camera;

	class SENTINEL_DLL Light : public GameComponent
	{
	protected:
		Transform* mTransform;

		std::vector<Drawable*> mDynamic;

		Texture* mTexture;
		RenderTexture* mRenderTexture;

	public:
		ColorRGBA mColor;
		Vector4 mAttenuation; // radius = w

	protected:
		Light();

	public:
		virtual ~Light();

		virtual void Startup();
		virtual void Shutdown();

		///////////////////////////////////////

		virtual void Draw(Camera* camera) = 0;

		Transform* GetTransform();

		Texture* GetTexture();
		RenderTexture* GetRenderTexture();

	protected:
		virtual void Save(Archive& archive);
		virtual void Load(Archive& archive);

		///////////////////////////////////////

		virtual void Copy(GameComponent* component);
	};
}}
