#include "WShaderManager.h"
#include "WString.h"
#include "ShaderManager.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	DEFINE_ASSET_MANAGER( Shader );

	bool WShaderManager::LoadConfig( System::String^ filename )
	{
		return ShaderManager::Inst()->LoadConfig( WString::Cast( filename ).c_str() );
	}
}}
