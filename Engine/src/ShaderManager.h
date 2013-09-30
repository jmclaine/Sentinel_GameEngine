#pragma once

#include "Singleton.h"
#include "Shader.h"

namespace Sentinel
{
	class SENTINEL_DLL ShaderManager : public SingletonSafe< ShaderManager >
	{
		friend class SingletonSafe< ShaderManager >;

	public:

		enum Result
		{
			SUCCESS,
			DUPLICATE_NAME,
			LOAD_FILE_FAILED,
		};

	private:

		std::unordered_map< std::string, Shader* > mData;

		//////////////////////////////

		ShaderManager();
		~ShaderManager();

	public:

		// Adds shaders to the ShaderManager from an XML file.
		//
		static ShaderManager* Load( const char* filename );

		Result		Add( Shader* shader, const std::string& name );

		void		Remove( const std::string& name );
		
		Shader*		Get( const std::string& name );

		void		GetAll( std::vector< std::string >& names, std::vector< const Shader* >& shaders );
	};
}
