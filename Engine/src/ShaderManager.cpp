#include "ShaderManager.h"
#include "Renderer.h"

namespace Sentinel
{
	ShaderManager::ShaderManager()
	{}

	ShaderManager::~ShaderManager()
	{
		TRAVERSE_LIST( it, mData )
			delete it->second;

		mData.clear();
	}

	ShaderManager::Result ShaderManager::Add( Shader* shader, const std::string& name )
	{
		if( !shader )
			throw AppException( "Failed to load " + name + " shader." );

		if( mData.find( name ) != mData.end() )
			return ShaderManager::DUPLICATE_NAME;

		mData.insert( std::pair< std::string, Shader* >( name, shader ));

		return ShaderManager::SUCCESS;
	}

	void ShaderManager::Remove( const std::string& name )
	{
		auto it = mData.find( name );

		if( it != mData.end() )
			mData.erase( it );
	}

	Shader* ShaderManager::Get( const std::string& name )
	{
		auto it = mData.find( name );

		if( it != mData.end() )
			return it->second;

		return NULL;
	}
}
