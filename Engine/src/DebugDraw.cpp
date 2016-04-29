#include "DebugDraw.h"
#include "MeshBuilder.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Shader.h"
#include "VertexLayout.h"
#include "Material.h"
#include "Exception.h"

namespace Sentinel
{
	DebugDraw::DebugDraw(
		Renderer* renderer,
		GameWorld* world,
		Component::Camera* camera,
		std::shared_ptr<Material> material,
		UINT maxLines) :

		mRenderer(renderer),
		mWorld(world),
		mCamera(camera),
		mMaterial(material),
		mMaxLines(maxLines)
	{
		_ASSERT(renderer);
		_ASSERT(camera);
		_ASSERT(material.get());
		_ASSERT(maxLines > 0);

		auto shader = material->mShader.lock();

		MeshBuilder builder;
		builder.mLayout = shader->Layout();

		for (UINT x = 0; x < maxLines; ++x)
		{
			UINT index = x << 1;

			builder.mVertex.push_back(MeshBuilder::Vertex(Vector3(0.0f, 0.0f, 0.0f)));
			builder.mVertex.push_back(MeshBuilder::Vertex(Vector3(0.0f, 0.0f, 0.0f)));
		}

		builder.mPrimitive = PrimitiveFormat::LINES;

		mMesh = builder.BuildMesh(mRenderer, false);

		if (!mMesh)
			throw AppException("Failed to create Mesh in Debug");
	}

	DebugDraw::~DebugDraw()
	{
		delete mMesh;
	}

	Renderer* DebugDraw::GetRenderer()
	{
		return mRenderer;
	}

	GameWorld* DebugDraw::GetWorld()
	{
		return mWorld;
	}

	/////////////////////////////////////

	void DebugDraw::Clear()
	{
		mNumLines = 0;
	}

	void DebugDraw::Line(const Vector3& start, const Vector3& end)
	{
		Vector3* data = (Vector3*)mMesh->mVertexBuffer->Lock() + (mNumLines << 1);

		*data = start;
		*(data + 1) = end;

		mMesh->mVertexBuffer->Unlock();

		++mNumLines;
	}

	void DebugDraw::Present()
	{
		mMesh->mMaterial = mMaterial;

		mMesh->Draw(mRenderer, mWorld, mCamera, mNumLines << 1);
	}
}
