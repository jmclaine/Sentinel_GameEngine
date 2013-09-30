#include "TextureManager.h"
#include "Util.h"

namespace Sentinel
{
	TextureManager::TextureManager()
	{}

	TextureManager::~TextureManager()
	{
		TRAVERSE_LIST( it, mData )
			delete it->second;

		mData.clear();
	}

	TextureManager::Result TextureManager::Add( Texture* texture, const std::string& name )
	{
		if( !texture )
			return TextureManager::LOAD_FILE_FAILED;

		if( mData.find( name ) != mData.end() )
			return TextureManager::DUPLICATE_NAME;

		mData.insert( std::pair< std::string, Texture* >( name, texture ));

		return TextureManager::SUCCESS;
	}

	void TextureManager::Remove( const std::string& name )
	{
		auto it = mData.find( name.c_str() );

		if( it != mData.end() )
			mData.erase( it );
	}

	Texture* TextureManager::Get( const std::string& name )
	{
		auto it = mData.find( name );

		if( it != mData.end() )
			return it->second;

		return NULL;
	}

	void TextureManager::GetAll( std::vector< std::string >& names, std::vector< const Texture* >& textures )
	{
		textures.clear();
		textures.reserve( mData.size() );
		
		names.clear();
		names.reserve( mData.size() );
		
		for( auto kv = mData.begin(); kv != mData.end(); ++kv )
		{
			names.push_back( kv->first );
			textures.push_back( kv->second );
		} 
	}
}
