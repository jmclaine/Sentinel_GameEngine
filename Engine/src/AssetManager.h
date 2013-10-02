#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "Common.h"
#include "Util.h"
#include "Singleton.h"

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
	};

	/////////////////////////////////////////////////////

#define DECLARE_MANAGER( refClass )\
	class refClass;\
	class SENTINEL_DLL refClass##Manager : public AssetManager< refClass >, public SingletonSafe< refClass##Manager >\
	{\
		friend class SingletonSafe< refClass##Manager >;\
		friend class AssetManager< refClass >;\
	private:\
		refClass##Manager();\
		~refClass##Manager();\
	public:\
		static refClass##Manager* Create();\
		static bool Load( const char* filename );\
	};

	DECLARE_MANAGER( Texture );
	DECLARE_MANAGER( Shader );
}
