#include "WShaderManager.h"
#include "ShaderManager.h"

namespace Sentinel { namespace Assets
{
	DEFINE_ASSET_MANAGER( Shader );

	bool WShaderManager::LoadConfig( System::String^ filename )
	{
		return ShaderManager::Inst()->LoadConfig( WString::Cast( filename ).c_str() );
	}
}}
