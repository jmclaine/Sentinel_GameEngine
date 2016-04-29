#pragma once

#include <memory>

#include "Sentinel.h"
#include "Renderer.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Shape.h"

namespace Sentinel
{
	namespace Component
	{
		class Camera;
	}

	class GameWorld;
	class Buffer;
	class Shader;
	class Texture;
	class Archive;
	class ShaderManager;
	class TextureManager;
	class MaterialManager;
	class VertexLayout;
	class Material;

	class SENTINEL_DLL Mesh
	{
		friend class MeshBuilder;

	public:
		PrimitiveFormat mPrimitive;

		Buffer* mVertexBuffer;
		Buffer* mIndexBuffer;

		std::weak_ptr<VertexLayout> mLayout;
		std::weak_ptr<Material> mMaterial;

		Matrix4x4 mMatrixWorld;

		BoundingBox mBounds;

		//////////////////////////////

		Mesh();
		~Mesh();

		void Draw(Renderer* renderer, GameWorld* world, Component::Camera* camera, UINT count = UINT_MAX);

		void CalculateBounds();

		//////////////////////////////

		static void Save(
			Archive& archive,
			Mesh* mesh,
			Renderer* renderer,
			MaterialManager* materialManager);

		static Mesh* Load(
			Archive& archive,
			Renderer* renderer,
			MaterialManager* materialManager);
	};
}
