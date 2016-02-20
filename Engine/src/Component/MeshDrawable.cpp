#include <vector>

#include "Component/MeshDrawable.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "Component/Transform.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "RenderManager.h"

namespace Sentinel {
namespace Component
{
	DEFINE_SERIAL_REGISTER(MeshDrawable);
	DEFINE_SERIAL_CLONE(MeshDrawable);

	MeshDrawable::MeshDrawable()
	{}

	MeshDrawable::MeshDrawable(std::shared_ptr<Mesh> mesh)
	{
		mMesh = mesh;
	}

	void MeshDrawable::Startup()
	{
		_ASSERT(mMesh);
		_ASSERT(mOwner);

		Drawable::Startup();

		if (mOwner->GetWorld())
			mOwner->GetWorld()->mRenderManager->Draw(this, mMesh->mMaterial->mRenderQueue, mOwner->mLayer);
	}

	///////////////////////////////////

	void MeshDrawable::Execute()
	{
		Drawable::Execute();
	}

	void MeshDrawable::CalculateBounds()
	{
		const BoundingBox& bounds = mMesh->mBounds;

		mBounds.Set(bounds.GetMinBounds(), bounds.GetMaxBounds(), mTransform->GetMatrixWorld());
	}

	bool MeshDrawable::CheckVisible(Camera* camera)
	{
		return camera->GetFrustum().Intersects(mBounds);
	}

	void MeshDrawable::Draw()
	{
		mMesh->mMatrixWorld = mTransform->GetMatrixWorld();

		GameWorld* world = mOwner->GetWorld();

		mMesh->Draw(world->mRenderer, world, world->mCurrentCamera);
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

		Drawable::Copy(drawable);

		return drawable;
	}
}}
