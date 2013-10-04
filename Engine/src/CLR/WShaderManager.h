#pragma once

#include "WAssetManager.h"
#include "WShader.h"

namespace Sentinel { namespace Assets
{
	public ref class WShaderManager
	{
		DECLARE_ASSET_MANAGER( Shader );

		static bool LoadConfig( System::String^ filename );
	};
}}
