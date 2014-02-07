#include "MeshManager.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Archive.h"

namespace Sentinel
{
	MeshManager::MeshManager()
	{}

	MeshManager::~MeshManager()
	{}

	void MeshManager::Save( Archive&			archive, 
							Renderer*			renderer, 
							ShaderManager*		shaderManager, 
							TextureManager*		textureManager,
							MaterialManager*	materialManager )
	{
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			archive.Write( &it->first );

			Mesh::Save( archive, it->second.get(), renderer, shaderManager, textureManager, materialManager );
		}
	}

	void MeshManager::Load(	Archive&			archive, 
							Renderer*			renderer, 
							ShaderManager*		shaderManager, 
							TextureManager*		textureManager,
							MaterialManager*	materialManager )
	{
		RemoveAll();

		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::string name;
			archive.Read( &name );

			std::shared_ptr< Mesh > mesh( Mesh::Load( archive, renderer, shaderManager, textureManager, materialManager ));

			if( !Add( name, mesh ))
				throw std::exception( "Failed to load mesh." );
		}
	}
}
