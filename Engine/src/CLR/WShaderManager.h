#pragma once

#include "Property.h"
#include "WAssetManager.h"
#include "ShaderManager.h"

namespace Sentinel { namespace Wrapped
{
	ref class WArchive;
	ref class WShader;
	ref class WRenderer;

	public ref class WShaderManager
	{
		DECLARE_ASSET_MANAGER( Shader );

	public:

		void	Save( WArchive^ archive );
		void	Load( WArchive^ archive, WRenderer^ renderer );

		static WShaderManager^ LoadConfig( System::String^ filename, WRenderer^ renderer, WShaderManager^ shaderManager );
	};
}}
