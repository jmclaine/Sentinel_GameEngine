#pragma once

#include "Memory.h"
#include "Vector3f.h"
#include "Types.h"

namespace Sentinel
{
	namespace Component
	{
		class Camera;
	}

	class Renderer;
	class GameWorld;
	class Shader;
	class Mesh;
	class Material;

	class SENTINEL_DLL DebugDraw
	{
	private:

		Renderer* mRenderer;
		GameWorld* mWorld;
		Component::Camera* mCamera;

		Mesh* mMesh;

		UINT mNumLines;
		UINT mMaxLines;

	public:

		std::shared_ptr<Material> mMaterial;

		DebugDraw(
			Renderer* renderer,
			GameWorld* world,
			Component::Camera* camera,
			std::shared_ptr<Material> material,
			UINT maxLines = 1000);

		~DebugDraw();

		Renderer* GetRenderer();
		GameWorld* GetWorld();

		/////////////////////////////////////

		void Clear();

		void Line(const Vector3f& start, const Vector3f& end);

		void Present();
	};
}