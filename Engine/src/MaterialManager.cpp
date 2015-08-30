#include "MaterialManager.h"
#include "Archive.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace Sentinel
{
	MaterialManager::MaterialManager()
	{}

	MaterialManager::~MaterialManager()
	{}

	/////////////////////////////////

	void MaterialManager::Save(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager)
	{
		UINT count = mData.size();
		archive.Write(&count);

		TRAVERSE_LIST(it, mData)
		{
			std::shared_ptr<Material> material = it->second;

			archive.Write(&it->first);

			Material::Save(archive, material.get(), renderer, shaderManager, textureManager);
		}
	}

	void MaterialManager::Load(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager)
	{
		RemoveAll();

		UINT count;
		archive.Read(&count);

		std::string name;
		for (UINT x = 0; x < count; ++x)
		{
			archive.Read(&name);

			std::shared_ptr<Material> material = std::shared_ptr<Material>(Material::Load(archive, renderer, shaderManager, textureManager));

			if (!Add(name, material))
				throw std::exception("Failed to load material.");
		}
	}
}
