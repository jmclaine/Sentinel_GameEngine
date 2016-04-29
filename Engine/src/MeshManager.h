#pragma once

#include "AssetManager.h"
#include "Mesh.h"

namespace Sentinel
{
	class Archive;
	class Renderer;
	class ShaderManager;
	class TextureManager;
	class BlendStateManager;

	class SENTINEL_DLL MeshManager : public AssetManager<Mesh>
	{
	public:
		MeshManager();
		MeshManager(const MeshManager&) = delete;
		MeshManager& operator = (const MeshManager&) = delete;

		/////////////////////////////////

		void Save(
			Archive& archive,
			Renderer* renderer,
			MaterialManager* materialManager);

		void Load(
			Archive& archive,
			Renderer* renderer,
			MaterialManager* materialManager);
	};
}
