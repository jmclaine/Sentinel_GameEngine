#include <vector>

#include "Component/MeshDrawable.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "Component/Transform.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "RenderManager.h"

namespace Sentinel
{
	namespace Component
	{
		SerialRegister MeshDrawable::SERIAL_REGISTER("MeshDrawable", MeshDrawable::Clone);
		Serializable* MeshDrawable::Clone() { return new MeshDrawable(); }

		MeshDrawable::MeshDrawable()
		{ }

		MeshDrawable::MeshDrawable(std::weak_ptr<Mesh>& mesh)
		{
			mMesh = mesh;
		}

		MeshDrawable::MeshDrawable(std::weak_ptr<Mesh>&& mesh)
		{
			mMesh = mesh;
		}

		void MeshDrawable::Startup()
		{
			_ASSERT(mOwner);

			Drawable::Startup();

			mOwner->GetWorld()->mRenderManager->Add(this, mMesh.lock()->mMaterial.lock()->mRenderQueue);
		}

		void MeshDrawable::Update()
		{
			Drawable::Update();
		}

		void MeshDrawable::Shutdown()
		{
			Drawable::Shutdown();

			mOwner->GetWorld()->mRenderManager->Remove(this, mMesh.lock()->mMaterial.lock()->mRenderQueue);
		}

		///////////////////////////////////

		void MeshDrawable::CalculateBounds()
		{
			const BoundingBox& bounds = mMesh.lock()->mBounds;

			mBounds.Set(bounds.GetMinBounds(), bounds.GetMaxBounds(), mTransform->GetMatrixWorld());
		}

		void MeshDrawable::Draw(Camera* camera)
		{
			auto mesh = mMesh.lock();

			GameWorld* world = mOwner->GetWorld();

			mesh->mMatrixWorld = mTransform->GetMatrixWorld();

			mesh->Draw(world->mRenderer, world, camera);
		}

		///////////////////////////////////

		void MeshDrawable::Save(Archive& archive)
		{
			_ASSERT(mOwner);
			_ASSERT(mOwner->GetWorld());
			_ASSERT(mOwner->GetWorld()->mMeshManager);

			SERIAL_REGISTER.Save(archive);

			Drawable::Save(archive);

			GameWorld* world = mOwner->GetWorld();

			archive.Write(&world->mMeshManager->Get(mMesh));
		}

		void MeshDrawable::Load(Archive& archive)
		{
			_ASSERT(mOwner);
			_ASSERT(mOwner->GetWorld());
			_ASSERT(mOwner->GetWorld()->mMeshManager);

			Drawable::Load(archive);

			GameWorld* world = mOwner->GetWorld();

			std::string name;
			archive.Read(&name);

			mMesh = world->mMeshManager->Get(name);
		}

		///////////////////////////////////

		GameComponent* MeshDrawable::Copy()
		{
			MeshDrawable* drawable = new MeshDrawable(mMesh);

			Copy(drawable);

			return drawable;
		}

		void MeshDrawable::Copy(GameComponent* component)
		{
			Drawable::Copy(component);

			MeshDrawable* drawable = static_cast<MeshDrawable*>(component);
			drawable->mMesh = mMesh;
		}
	}
}
