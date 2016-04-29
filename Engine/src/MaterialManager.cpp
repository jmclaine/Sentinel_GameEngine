#include "MaterialManager.h"
#include "Archive.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace Sentinel
{
	MaterialManager::MaterialManager()
	{ }

	/////////////////////////////////

	void MaterialManager::Save(
		Archive& archive,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		BlendStateManager* blendManager)
	{
		UINT count = mData.size();
		archive.Write(&count);

		for (auto it : mData)
		{
			std::shared_ptr<Material> material = it.second;

			archive.Write(&it.first);

			Material::Save(archive, material.get(), shaderManager, textureManager, blendManager);
		}
	}

	void MaterialManager::Load(
		Archive& archive,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		BlendStateManager* blendManager)
	{
		RemoveAll();

		UINT count;
		archive.Read(&count);

		std::string name;
		for (UINT x = 0; x < count; ++x)
		{
			archive.Read(&name);

			auto material = std::shared_ptr<Material>(Material::Load(archive, shaderManager, textureManager, blendManager));

			Add(name, material);
		}
	}
}
