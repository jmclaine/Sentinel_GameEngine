#pragma once

#include "AssetManager.h"
#include "Model.h"

namespace Sentinel
{
	class Renderer;
	class ShaderManager;
	class TextureManager;

	class SENTINEL_DLL ModelManager : public AssetManager<Model>
	{
	public:
		ModelManager();
		ModelManager(const ModelManager&) = delete;
		ModelManager& operator = (const ModelManager&) = delete;

		/////////////////////////////////

		void Save(
			Archive& archive,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager,
			BlendStateManager* blendManager);

		void Load(
			Archive& archive,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager,
			BlendStateManager* blendManager);
	};
}
