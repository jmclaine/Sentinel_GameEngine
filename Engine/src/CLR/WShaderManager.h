#pragma once

#include "WShader.h"

namespace Sentinel { namespace Assets
{
	public ref class WShaderManager
	{
	public:

		enum class Result
		{
			SUCCESS,
			DUPLICATE_NAME,
		};

		static Result	Add( WShader^ shader, System::String^ name );

		static void		Remove( System::String^ name );
		
		static WShader^	Get( System::String^ name );
	};
}}
