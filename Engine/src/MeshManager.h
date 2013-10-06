#pragma once

#include "AssetManager.h"
#include "Mesh.h"

namespace Sentinel
{
	class SENTINEL_DLL MeshManager : public AssetManager< Mesh >, public SingletonSafe< MeshManager >
	{
		friend class SingletonSafe< MeshManager >;
		friend class AssetManager< Mesh >;
		
	private:

		MeshManager();
		~MeshManager();

	public:

		static MeshManager* Create();

		void Save( Archive& archive );

		void Load( Archive& archive );

	private:

		static void SaveMesh( Archive& archive, std::shared_ptr< Mesh > mesh );

		static void LoadMesh( Archive& archive, std::shared_ptr< Mesh > mesh );
	};
}
