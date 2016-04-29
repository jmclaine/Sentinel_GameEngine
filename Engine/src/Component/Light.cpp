#include "Light.h"
#include "GameWorld.h"
#include "Component/Transform.h"
#include "GameObject.h"
#include "Archive.h"
#include "Texture.h"
#include "Renderer.h"
#include "Exception.h"
#include "RenderTexture.h"

namespace Sentinel
{
	namespace Component
	{
		Light::Light() :
			mAttenuation(Vector4(1.0f, 0.0f, 0.0f, 25.0f)),
			mTexture(nullptr),
			mRenderTexture(nullptr)
		{ }

		Light::~Light()
		{
			SAFE_DELETE(mTexture);
			SAFE_DELETE(mRenderTexture);
		}

		Transform* Light::GetTransform()
		{
			return mTransform;
		}

		Texture* Light::GetTexture()
		{
			return mTexture;
		}

		RenderTexture* Light::GetRenderTexture()
		{
			return mRenderTexture;
		}

		///////////////////////////////////////

		void Light::Startup()
		{
			_ASSERT(mOwner);

			mTransform = mOwner->mTransform;

			if (mTransform == nullptr)
				throw AppException("Light::Startup()\n" + std::string(mOwner->mName) + " does not contain Transform");

			Sentinel::GameWorld* world = mOwner->GetWorld();

			_ASSERT(world);

			world->AddLight(this);
		}

		void Light::Shutdown()
		{
			mTransform = nullptr;
		}

		///////////////////////////////////////

		void Light::Save(Archive& archive)
		{
			GameComponent::Save(archive);

			archive.Write(&mColor);
			archive.Write(&mAttenuation);
		}

		void Light::Load(Archive& archive)
		{
			GameComponent::Load(archive);

			archive.Read(&mColor);
			archive.Read(&mAttenuation);
		}

		///////////////////////////////////////

		void Light::Copy(GameComponent* component)
		{
			GameComponent::Copy(component);

			auto light = static_cast<Light*>(component);
			light->mColor = mColor;
			light->mAttenuation = mAttenuation;
		}
	}
}
