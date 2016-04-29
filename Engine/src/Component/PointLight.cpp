#include "Component/PointLight.h"
#include "Component/Transform.h"
#include "Archive.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "Renderer.h"
#include "MaterialManager.h"
#include "Texture.h"
#include "Component/Drawable.h"

namespace Sentinel
{
	namespace Component
	{
		SerialRegister PointLight::SERIAL_REGISTER("PointLight", PointLight::Clone);
		Serializable* PointLight::Clone() { return new PointLight(); }

		PointLight::PointLight() :
			mResolution(1024)
		{ }

		PointLight::PointLight(UINT resolution) :
			mResolution(resolution)
		{ }

		PointLight::~PointLight()
		{ }

		void PointLight::Startup()
		{
			_ASSERT(mOwner);

			Light::Startup();

			Sentinel::GameWorld* world = mOwner->GetWorld();

			_ASSERT(world);

			Renderer* renderer = world->mRenderer;

			_ASSERT(renderer);

			if (!mTexture)
				mTexture = renderer->CreateTextureCube(nullptr, mResolution, mResolution, ImageFormat::RG);

			if (!mRenderTexture)
				mRenderTexture = renderer->CreateRenderTexture(mTexture);
		}

		void PointLight::Update()
		{
			float resolution = static_cast<float>(mResolution);
			mMatrixProjection.ProjectionPerspective(resolution, resolution, 0.0f, mAttenuation.w, 90.0f);

			const Vector3& pos = mTransform->GetMatrixWorld().Transform(Vector3(0, 0, 0));

			mMatrixView[CAMERA_AXIS_POS_X].LookAtView(pos, pos + Vector3(1, 0, 0), Vector3(0, -1, 0));
			mMatrixFinal[CAMERA_AXIS_POS_X] = mMatrixProjection * mMatrixView[CAMERA_AXIS_POS_X];

			mMatrixView[CAMERA_AXIS_NEG_X].LookAtView(pos, pos + Vector3(-1, 0, 0), Vector3(0, -1, 0));
			mMatrixFinal[CAMERA_AXIS_NEG_X] = mMatrixProjection * mMatrixView[CAMERA_AXIS_NEG_X];

			mMatrixView[CAMERA_AXIS_POS_Y].LookAtView(pos, pos + Vector3(0, 1, 0), Vector3(0, 0, 1));
			mMatrixFinal[CAMERA_AXIS_POS_Y] = mMatrixProjection * mMatrixView[CAMERA_AXIS_POS_Y];

			mMatrixView[CAMERA_AXIS_NEG_Y].LookAtView(pos, pos + Vector3(0, -1, 0), Vector3(0, 0, -1));
			mMatrixFinal[CAMERA_AXIS_NEG_Y] = mMatrixProjection * mMatrixView[CAMERA_AXIS_NEG_Y];

			mMatrixView[CAMERA_AXIS_POS_Z].LookAtView(pos, pos + Vector3(0, 0, 1), Vector3(0, -1, 0));
			mMatrixFinal[CAMERA_AXIS_POS_Z] = mMatrixProjection * mMatrixView[CAMERA_AXIS_POS_Z];

			mMatrixView[CAMERA_AXIS_NEG_Z].LookAtView(pos, pos + Vector3(0, 0, -1), Vector3(0, -1, 0));
			mMatrixFinal[CAMERA_AXIS_NEG_Z] = mMatrixProjection * mMatrixView[CAMERA_AXIS_NEG_Z];

			GameWorld* world = mOwner->GetWorld();
			UINT count = world->NumGameObjects();

			mDynamic.clear();

			for (UINT x = 0; x < count; ++x)
			{
				AddDynamic(world->GetGameObject(x));
			}
		}

		void PointLight::Shutdown()
		{
			Light::Shutdown();
		}

		///////////////////////////////////////

		void PointLight::Draw(Camera* camera)
		{
			auto world = mOwner->GetWorld();
			auto renderer = world->mRenderer;

			static float color[4] = { 1, 1, 1, 1 };

			renderer->SetViewport(0, 0, mResolution, mResolution);
			renderer->SetDepthStencil(nullptr);
			renderer->SetRenderTexture(mRenderTexture);
			renderer->ClearColor(color);

			mMaterial.lock()->Apply(renderer);

			Material::Lock();

			UINT count = (UINT)mDynamic.size();
			for (UINT x = 0; x < count; ++x)
			{
				mDynamic[x]->Draw(camera);
			}

			Material::Unlock();
		}

		const Matrix4x4& PointLight::GetMatrixFinal(CameraAxisType axis)
		{
			return mMatrixFinal[axis];
		}

		float* PointLight::PtrMatrixFinal()
		{
			return mMatrixFinal[0].Ptr();
		}

		void PointLight::AddDynamic(GameObject* obj)
		{
			auto drawable = obj->GetComponent<Drawable>();

			if (drawable)
			{
				if (drawable->mIsDynamic)
				{
					if (drawable->mBounds.Intersects(BoundingSphere(mTransform->mPosition, mAttenuation.w)))
					{
						mDynamic.push_back(drawable);
					}
				}
			}

			UINT count = obj->NumChildren();
			for (UINT x = 0; x < count; ++x)
			{
				AddDynamic(obj->GetChild(x));
			}
		}

		///////////////////////////////////////

		void PointLight::Save(Archive& archive)
		{
			SERIAL_REGISTER.Save(archive);

			Light::Save(archive);

			archive.Write(&mResolution);

			archive.Write(&mOwner->GetWorld()->mMaterialManager->Get(mMaterial));
		}

		void PointLight::Load(Archive& archive)
		{
			Light::Load(archive);

			archive.Read(&mResolution);

			std::string material;
			archive.Read(&material);

			mMaterial = mOwner->GetWorld()->mMaterialManager->Get(material);
		}

		///////////////////////////////////////

		GameComponent* PointLight::Copy()
		{
			auto light = new PointLight(mResolution);

			Copy(light);

			return light;
		}

		void PointLight::Copy(GameComponent* component)
		{
			Light::Copy(component);

			auto light = static_cast<PointLight*>(component);
			light->mMaterial = mMaterial;
		}
	}
}
