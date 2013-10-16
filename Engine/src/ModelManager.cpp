#include "ModelManager.h"
#include "Archive.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace Sentinel
{
	ModelManager::ModelManager()
	{}

	ModelManager::~ModelManager()
	{}

	void ModelManager::Save( Archive& archive, 
							 Renderer* renderer, 
							 ShaderManager* shaderManager, 
							 TextureManager* textureManager )
	{
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			archive.Write( &it->first );

			it->second->Save( archive, renderer, shaderManager, textureManager );
		}
	}

	void ModelManager::Load( Archive& archive, 
							 Renderer* renderer, 
							 ShaderManager* shaderManager, 
							 TextureManager* textureManager )
	{
		RemoveAll();

		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::string name;
			archive.Read( &name );

			if( !Add( name, std::shared_ptr< Model >( Model::Load( archive, renderer, shaderManager, textureManager ))))
				throw std::exception( "Failed to load mesh." );
		}
	}
}
