#pragma once

#include "WShader.h"

using namespace System::Collections::Generic;

namespace Sentinel { namespace Assets
{
	public ref class WShaderManager
	{
	public:

		enum class Result
		{
			SUCCESS,
			DUPLICATE_NAME,
			LOAD_FILE_FAILED,
		};

		static bool		Load( System::String^ filename );

		static Result	Add( WShader^ shader, System::String^ name );

		static void		Remove( System::String^ name );
		
		static WShader^	Get( System::String^ name );

		static void		GetAll( List< System::String^ >^% names, List< WShader^ >^% shaders );
	};
}}
