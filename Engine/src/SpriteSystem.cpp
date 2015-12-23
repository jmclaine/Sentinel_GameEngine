#include "SpriteSystem.h"
#include "MeshBuilder.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Material.h"
#include "Sprite.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Component/Camera.h"
#include "Component/Transform.h"
#include "VertexLayout.h"
#include "Material.h"

namespace Sentinel
{
	SpriteSystem::SpriteSystem(Renderer* renderer, std::shared_ptr<VertexLayout> layout, UINT maxSprites) :
		mRenderer(renderer),
		mMaxSprites(maxSprites),
		mNumSprites(0)
	{
		_ASSERT(renderer);
		_ASSERT(layout.get());
		_ASSERT(maxSprites > 0);

		mStorage = new Storage[maxSprites];

		MeshBuilder builder;

		builder.mLayout = layout;

		for (UINT x = 0; x < maxSprites; ++x)
		{
			builder.mVertex.push_back(MeshBuilder::Vertex(Vector3(0.0f, 0.0f, 0.0f)));
		}

		builder.mPrimitive = PrimitiveFormat::POINTS;

		mMesh = builder.BuildMesh(mRenderer, false);

		if (!mMesh)
			throw AppException("Failed to create Mesh in SpriteSystem");
	}

	SpriteSystem::~SpriteSystem()
	{
		delete[] mStorage;

		delete mMesh;
	}

	Renderer* SpriteSystem::GetRenderer()
	{
		return mRenderer;
	}

	/////////////////////////////////////

	void SpriteSystem::Clear()
	{
		mNumSprites = 0;
	}

	void SpriteSystem::Draw(UINT frame, const ColorRGBA& color, const Matrix4x4& matWorld)
	{
		_ASSERT(mNumSprites < mMaxSprites);

		Storage& store = mStorage[mNumSprites];

		store.mFrame = mSprite->GetFrame(frame);
		store.mColor = color.ToUINT();
		store.mMatrixWorld = matWorld;

		++mNumSprites;
	}

	void SpriteSystem::Present()
	{
		_ASSERT(mCamera);

		memcpy(mMesh->mVertexBuffer->Lock(), mStorage, sizeof(Storage) * mNumSprites);

		mMesh->mVertexBuffer->Unlock();

		mMesh->mMaterial = mMaterial;

		mMesh->Draw(mRenderer, NULL, mCamera, mNumSprites);
	}
}
