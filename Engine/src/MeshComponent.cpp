#include <vector>

#include "Renderer.h"
#include "GameWorld.h"
#include "Timing.h"
#include "LightComponent.h"
#include "MeshComponent.h"

namespace Sentinel
{
	MeshComponent::MeshComponent( std::shared_ptr< Mesh > mesh, const Material& material )
	{
		mMesh = mesh;
		mMaterial = material;
	}

	void MeshComponent::Startup()
	{
		DrawableComponent::Startup();
	}

	void MeshComponent::Update()
	{
		DrawableComponent::Update();

		mMesh->mMaterial = mMaterial;

		mMesh->mMatrixWorld = mMatrixTranslation * mMatrixRotation * mMatrixScale;

		mMesh->Draw();
	}

	void MeshComponent::Shutdown()
	{
		mMesh.reset();
	}
}
