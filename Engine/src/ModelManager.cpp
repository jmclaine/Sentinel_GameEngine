#include "ModelManager.h"
#include "Archive.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace Sentinel
{
	ModelManager::ModelManager()
	{ }

	void ModelManager::Save(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager)
	{
		UINT count = mData.size();
		archive.Write(&count);

		TRAVERSE_LIST(it, mData)
		{
			archive.Write(&it->first);

			it->second->Save(
				archive, 
				renderer, 
				shaderManager, 
				textureManager, 
				materialManager, 
				blendManager);
		}
	}

	void ModelManager::Load(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager)
	{
		RemoveAll();

		UINT count;
		archive.Read(&count);

		for (UINT x = 0; x < count; ++x)
		{
			std::string name;
			archive.Read(&name);

			Add(name, std::shared_ptr<Model>(Model::Load(archive, renderer, shaderManager, textureManager, materialManager, blendManager)));
		}
	}
}
