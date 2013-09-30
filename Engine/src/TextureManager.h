#pragma once

#include "Common.h"
#include "Singleton.h"
#include "Texture.h"

namespace Sentinel
{
	class SENTINEL_DLL TextureManager : public SingletonSafe< TextureManager >
	{
		friend class SingletonSafe< TextureManager >;

	public:

		enum Result
		{
			SUCCESS,
			DUPLICATE_NAME,
			LOAD_FILE_FAILED,
		};

	private:

		std::unordered_map< std::string, Texture* > mData;

		//////////////////////////////

		TextureManager();
		~TextureManager();

	public:

		Result		Add( Texture* texture, const std::string& name );

		void		Remove( const std::string& name );
		
		Texture*	Get( const std::string& name );

		void		GetAll( std::vector< std::string >& names, std::vector< const Texture* >& textures );
	};
}
