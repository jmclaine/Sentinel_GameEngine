#pragma once

#include "AssetManager.h"
#include "Shader.h"

namespace Sentinel
{
	class Renderer;
	class Archive;

	class SENTINEL_DLL ShaderManager : public AssetManager<Shader>
	{
	public:
		ShaderManager();

		ShaderManager(const ShaderManager&) = delete;
		ShaderManager& operator = (const ShaderManager&) = delete;

		/////////////////////////////////

		void Save(Archive& archive);
		void Load(Archive& archive, Renderer* renderer);
	};
}
