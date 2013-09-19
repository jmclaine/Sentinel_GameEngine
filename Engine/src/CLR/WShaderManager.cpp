#include "RString.h"
#include "WShaderManager.h"
#include "ShaderManager.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	bool WShaderManager::Load( System::String^ filename )
	{
		ShaderManager* obj = ShaderManager::Load( RString::ToString( filename ).c_str() );

		if( !obj )
			return false;

		ShaderManager::Inst( obj );

		return true;
	}

	WShaderManager::Result WShaderManager::Add( WShader^ shader, System::String^ name )
	{
		return (WShaderManager::Result)ShaderManager::Inst()->Add( shader, RString::ToString( name ));
	}

	void WShaderManager::Remove( System::String^ name )
	{
		ShaderManager::Inst()->Remove( RString::ToString( name ));
	}
	
	WShader^ WShaderManager::Get( System::String^ name )
	{
		return gcnew RShader( ShaderManager::Inst()->Get( RString::ToString( name )));
	}
}}
