#pragma once

#include "AssetManager.h"
#include "Material.h"

namespace Sentinel
{
	class Archive;
	class Renderer;
	class ShaderManager;
	class TextureManager;
	class BlendStateManager;

	class SENTINEL_DLL MaterialManager : public AssetManager<Material>
	{
	public:
		MaterialManager();

		MaterialManager(const MaterialManager&) = delete;
		MaterialManager& operator = (const MaterialManager&) = delete;

		/////////////////////////////////

		void Save(
			Archive& archive,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			BlendStateManager* blendManager);

		void Load(
			Archive& archive,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			BlendStateManager* blendManager);
	};
}
