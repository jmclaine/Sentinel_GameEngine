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

#include "Vector3f.h"
#include "Matrix4f.h"
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

		Component::Camera* mCamera;

		struct Storage
		{
		public:

			Quad mFrame;
			UINT mColor;
			Matrix4f mMatrixWorld;
		};

	private:

		Storage* mStorage;

	public:

		std::shared_ptr<Sprite> mSprite;
		std::shared_ptr<Material> mMaterial;

		/////////////////////////////////////

		SpriteSystem(Renderer* renderer, std::shared_ptr<VertexLayout> layout, UINT maxSprites);
		~SpriteSystem();

		Renderer* GetRenderer();

		/////////////////////////////////////

		void Clear();

		void Draw(UINT frame, const ColorRGBA& color, const Matrix4f& matWorld);

		void Present();
	};
}
