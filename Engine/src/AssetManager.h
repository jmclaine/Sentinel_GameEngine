#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Common.h"
#include "Archive.h"
#include "Util.h"

namespace Sentinel
{
	template< class T >
	class AssetManager
	{
	protected:

		std::unordered_map< std::string, std::shared_ptr< T >> mData;

		AssetManager() {}

		virtual ~AssetManager()
		{
			mData.clear();
		}

	public:

		std::shared_ptr< T > Add( const std::string& name, std::shared_ptr< T > data )
		{
			if( !data )
				return NULL;

			if( mData.find( name ) != mData.end() )
				return NULL;

			mData.insert( std::pair< std::string, std::shared_ptr< T >>( name, data ));

			return data;
		}

		void Remove( const std::string& name )
		{
			auto it = mData.find( name );

			if( it != mData.end() )
				mData.erase( it );
		}

		std::shared_ptr< T > Get( const std::string& name )
		{
			auto it = mData.find( name );

			if( it != mData.end() )
				return it->second;
			else
				return NULL;
		}

		std::string Get( std::shared_ptr< T > data )
		{
			TRAVERSE_LIST( it, mData )
			{
				if( it->second.get() == data.get() )
					return it->first;
			}

			return "";
		}

		void GetAll( std::vector< std::string >& names, std::vector< std::shared_ptr< T >>& data )
		{
			data.clear();
			data.reserve( mData.size() );
		
			names.clear();
			names.reserve( mData.size() );
		
			for( auto kv = mData.begin(); kv != mData.end(); ++kv )
			{
				names.push_back( kv->first );
				data.push_back( kv->second );
			}
		}

		virtual void Save( Archive& archive ) = 0;

		virtual void Load( Archive& archive ) = 0;
	};
}
