#include "WShaderManager.h"
#include "WShader.h"
#include "WRenderer.h"
#include "WArchive.h"
#include "WString.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_ASSET_MANAGER( Shader );

	void WShaderManager::Save( WArchive^ archive )
	{
		mRef->Save( *archive );
	}

	void WShaderManager::Load( WArchive^ archive, WRenderer^ renderer )
	{
		mRef->Load( *archive, renderer );
	}

	WShaderManager^ WShaderManager::LoadConfig( System::String^ filename, WRenderer^ renderer, WShaderManager^ shaderManager )
	{
		if( !ShaderManager::LoadConfig( WString::Cast( filename ).c_str(), renderer, shaderManager ))
			return nullptr;

		return shaderManager;
	}
}}
