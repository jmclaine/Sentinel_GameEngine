#pragma once

#include "Singleton.h"
#include "Shader.h"

namespace Sentinel
{
	class SENTINEL_DLL ShaderManager : public Singleton< ShaderManager >
	{
		friend class Singleton< ShaderManager >;

	public:

		enum Result
		{
			SUCCESS,
			DUPLICATE_NAME,
		};

	private:

		std::unordered_map< std::string, Shader* > mData;

		//////////////////////////////

		ShaderManager();
		~ShaderManager();

	public:

		Result		Add( Shader* shader, const std::string& name );

		void		Remove( const std::string& name );
		
		Shader*		Get( const std::string& name );
	};
}
