#include "MeshManager.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Archive.h"

namespace Sentinel
{
	MeshManager::MeshManager()
	{ }

	void MeshManager::Save(
		Archive& archive,
		Renderer* renderer,
		MaterialManager* materialManager)
	{
		UINT count = mData.size();
		archive.Write(&count);

		TRAVERSE_LIST(it, mData)
		{
			archive.Write(&it->first);

			Mesh::Save(archive, it->second.get(), renderer, materialManager);
		}
	}

	void MeshManager::Load(
		Archive& archive,
		Renderer* renderer,
		MaterialManager* materialManager)
	{
		RemoveAll();

		UINT count;
		archive.Read(&count);

		for (UINT x = 0; x < count; ++x)
		{
			std::string name;
			archive.Read(&name);

			std::shared_ptr<Mesh> mesh(Mesh::Load(archive, renderer, materialManager));

			Add(name, mesh);
		}
	}
}
