#pragma once
/*
Mesh is fully customizable.

Mesh creation helpers are in MeshBuilder.h or Model.h
*/
#include <memory>

#include "Sentinel.h"
#include "Types.h"
#include "RendererTypes.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Shape.h"

namespace Sentinel
{
	namespace Component
	{
		class Camera;
	}

	class Renderer;
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

		PrimitiveFormat::Type mPrimitive;

		Buffer* mVertexBuffer;
		Buffer* mIndexBuffer;

		std::shared_ptr<VertexLayout> mLayout;
		std::shared_ptr<Material> mMaterial;

		Matrix4x4 mMatrixWorld;

		BoundingBox mBounds;

		//////////////////////////////

		Mesh();
		~Mesh();

		void Draw(Renderer* renderer, GameWorld* world, Component::Camera* camera, UINT count = UINT_MAX);

		// Helper function to calculate the bounding box using the VBO.
		//
		void CalculateBounds();

		//////////////////////////////

		static void Save(
			Archive& archive,
			Mesh* mesh,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager);

		static Mesh* Load(
			Archive& archive,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager);
	};
}
