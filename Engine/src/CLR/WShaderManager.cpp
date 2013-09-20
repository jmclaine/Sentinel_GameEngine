#include "WString.h"
#include "WShaderManager.h"
#include "ShaderManager.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	bool WShaderManager::Load( System::String^ filename )
	{
		ShaderManager* obj = ShaderManager::Load( msclr::interop::marshal_as< std::string >( filename ).c_str() );

		if( !obj )
			return false;

		ShaderManager::Inst( obj );

		return true;
	}

	WShaderManager::Result WShaderManager::Add( WShader^ shader, System::String^ name )
	{
		return (WShaderManager::Result)ShaderManager::Inst()->Add( shader, msclr::interop::marshal_as< std::string >(name).c_str() );
	}

	void WShaderManager::Remove( System::String^ name )
	{
		ShaderManager::Inst()->Remove( msclr::interop::marshal_as< std::string >(name).c_str() );
	}
	
	WShader^ WShaderManager::Get( System::String^ name )
	{
		return gcnew RShader( ShaderManager::Inst()->Get( msclr::interop::marshal_as< std::string >(name).c_str() ));
	}

	void WShaderManager::GetAll( List< System::String^ >^% names, List< WShader^ >^% shaders )
	{
		std::vector< std::string > _names;
		std::vector< const Shader* > _shaders;

		ShaderManager::Inst()->GetAll( _names, _shaders );

		TRAVERSE_VECTOR( x, _names )
			names->Add( WString::Cast( _names[ x ].c_str() ));

		TRAVERSE_VECTOR( x, _shaders )
			shaders->Add( gcnew RShader( const_cast< Shader* >(_shaders[ x ]) ));
	}
}}
