#include "ModelManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace Sentinel
{
	ModelManager::ModelManager()
	{}

	ModelManager::~ModelManager()
	{}

	ModelManager* ModelManager::Create()
	{
		return ModelManager::Inst();
	}

	void ModelManager::Save( Archive& archive )
	{
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			archive.Write( &it->first );

			it->second->Save( archive );
		}
	}

	void ModelManager::Load( Archive& archive )
	{
		RemoveAll();

		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::string name;
			archive.Read( &name );

			if( !Add( name, std::shared_ptr< Model >( Model::Load( archive ))))
				throw std::exception( "Failed to load mesh." );
		}
	}
}
