#pragma once

#include "WString.h"
#include "WTexture.h"
#include "WShader.h"

using namespace System::Collections::Generic;

using namespace Sentinel::Assets;

namespace Sentinel { namespace Utilities
{

#define DECLARE_ASSET_MANAGER( refClass )\
	public ref class W##refClass##Manager sealed\
	{\
	public:\
		static W##refClass^ Add( System::String^ name, W##refClass^ data );\
		static void Remove( System::String^ name );\
		static W##refClass^ Get( System::String^ name );\
		static void GetAll( List< System::String^ >^% names, List< W##refClass^ >^% data );\
		static void Create();\
		static bool Load( System::String^ filename );\
	};

	DECLARE_ASSET_MANAGER( Texture );
	DECLARE_ASSET_MANAGER( Shader );
}}
