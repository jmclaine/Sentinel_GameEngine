#include "Component/ModelDrawable.h"
#include "Model.h"
#include "ModelManager.h"
#include "Component/Transform.h"
#include "Material.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "RenderManager.h"
#include "Timing.h"

namespace Sentinel
{
	namespace Component
	{
		SerialRegister ModelDrawable::SERIAL_REGISTER("ModelDrawable", ModelDrawable::Clone);
		Serializable* ModelDrawable::Clone() { return new ModelDrawable(); }

		ModelDrawable::ModelDrawable()
		{ }

		ModelDrawable::ModelDrawable(std::weak_ptr<Model>& model)
		{
			mModel = model;
		}

		ModelDrawable::ModelDrawable(std::weak_ptr<Model>&& model)
		{
			mModel = model;
		}

		void ModelDrawable::Startup()
		{
			Drawable::Startup();

			mOwner->GetWorld()->mRenderManager->Add(this, (WORD)RenderQueue::GEOMETRY);
		}

		void ModelDrawable::Update()
		{
			Drawable::Update();
		}

		void ModelDrawable::Shutdown()
		{
			Drawable::Shutdown();

			mOwner->GetWorld()->mRenderManager->Remove(this, (WORD)RenderQueue::GEOMETRY);
		}

		///////////////////////////////////

		void ModelDrawable::CalculateBounds()
		{ }

		bool ModelDrawable::CheckVisible(Camera* camera)
		{
			return true;
		}

		void ModelDrawable::Draw(Camera* camera)
		{
			GameWorld* world = mOwner->GetWorld();

			auto model = mModel.lock();

			model->mMatrixWorld = mTransform->GetMatrixWorld();
			model->Update(world->mTiming->DeltaTime());

			model->Draw(world->mRenderer, world, camera);
		}

		///////////////////////////////////

		void ModelDrawable::Save(Archive& archive)
		{
			_ASSERT(mOwner);
			_ASSERT(mOwner->GetWorld());
			_ASSERT(mOwner->GetWorld()->mModelManager);

			SERIAL_REGISTER.Save(archive);

			Drawable::Save(archive);

			GameWorld* world = mOwner->GetWorld();

			archive.Write(&world->mModelManager->Get(mModel));
		}

		void ModelDrawable::Load(Archive& archive)
		{
			_ASSERT(mOwner);
			_ASSERT(mOwner->GetWorld());
			_ASSERT(mOwner->GetWorld()->mModelManager);

			Drawable::Load(archive);

			GameWorld* world = mOwner->GetWorld();

			std::string name;
			archive.Read(&name);

			mModel = world->mModelManager->Get(name);
		}

		///////////////////////////////////

		GameComponent* ModelDrawable::Copy()
		{
			ModelDrawable* drawable = new ModelDrawable(mModel);

			Copy(drawable);

			return drawable;
		}

		void ModelDrawable::Copy(GameComponent* component)
		{
			Drawable::Copy(component);
		}
	}
}
