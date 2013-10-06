#pragma once

#include "WAssetManager.h"
#include "WShader.h"

namespace Sentinel { namespace Assets
{
	public ref class WShaderManager
	{
	public:

		DECLARE_ASSET_MANAGER( Shader );

		static bool LoadConfig( System::String^ filename );
	};
}}
