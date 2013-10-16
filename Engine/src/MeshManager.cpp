#include "MeshManager.h"
#include "AssetArchive.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace Sentinel
{
	MeshManager::MeshManager()
	{}

	MeshManager::~MeshManager()
	{}

	void MeshManager::Save( Archive&		archive, 
							Renderer*		renderer, 
							ShaderManager*	shaderManager, 
							TextureManager*	textureManager )
	{
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			archive.Write( &it->first );

			AssetArchive::SaveMesh( archive, it->second.get(), renderer, shaderManager, textureManager );
		}
	}

	void MeshManager::Load(	Archive&		archive, 
							Renderer*		renderer, 
							ShaderManager*	shaderManager, 
							TextureManager*	textureManager )
	{
		RemoveAll();

		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::string name;
			archive.Read( &name );

			std::shared_ptr< Mesh > mesh( AssetArchive::LoadMesh( archive, renderer, shaderManager, textureManager ));

			if( !Add( name, mesh ))
				throw std::exception( "Failed to load mesh." );
		}
	}
}
