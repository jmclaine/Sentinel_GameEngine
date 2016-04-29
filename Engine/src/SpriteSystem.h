#pragma once
/*
Creates a point mesh to generate quads using a geometry shader.

Use 'sprite.xsh' as a basis to create a custom shader.
Be aware that the vertex attributes must remain the same
to ensure the SpriteSystem can modify and render the
quads correctly.

Uniforms can be added freely.

Use the current GameWorld camera to render sprites.
*/
#include <memory>

#include "Vector3.h"
#include "Matrix4x4.h"
#include "ColorRGBA.h"
#include "Point.h"

namespace Sentinel
{
	namespace Component
	{
		class Camera;
	}

	class Renderer;
	class Mesh;
	class Sprite;
	class Material;
	class VertexLayout;

	class SENTINEL_DLL SpriteSystem
	{
	private:
		Renderer* mRenderer;
		Mesh* mMesh;

		UINT mNumSprites;
		UINT mMaxSprites;

	public:
		struct Storage
		{
			Quad mFrame;
			UINT mColor;
			Matrix4x4 mMatrixWorld;
		};

	private:
		Storage* mStorage;

	public:
		std::weak_ptr<Sprite> mSprite;
		std::weak_ptr<Material> mMaterial;

		/////////////////////////////////////

		SpriteSystem(Renderer* renderer, std::weak_ptr<VertexLayout>& layout, UINT maxSprites);
		~SpriteSystem();

		SpriteSystem(const SpriteSystem&) = delete;
		SpriteSystem& operator = (const SpriteSystem&) = delete;

		Renderer* GetRenderer();

		/////////////////////////////////////

		void Clear();
		void Draw(UINT frame, const ColorRGBA& color, const Matrix4x4& matWorld);
		void Present(Component::Camera* camera);
	};
}
