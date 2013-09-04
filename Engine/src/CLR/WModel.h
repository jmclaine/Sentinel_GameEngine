#pragma once

#include "m_shared_ptr.h"
#include "Property.h"
#include "Model.h"
#include "WShader.h"
#include "WMatrix4f.h"
#include "WMaterial.h"

using namespace Sentinel::Math;

namespace Sentinel { namespace Assets
{
	public ref class WModel
	{
		DECLARE_REF_SHARED( Model );

	public:

		DECLARE_PROPERTY( WMatrix4f^, MatrixWorld );

		////////////////////////////////

		static WModel^		Load( System::String^ filename );

		static void			SetShaderColor( WShader^ shader );
		static WShader^		GetShaderColor();

		bool				Create( System::String^ filename );
		void				Release();

		void				SetMaterial( WMaterial^ material );
		void				SetShader( WShader^ shader );

		//void				SetKeyFrame( const KeyFrame& key, int keyIndex = -1, int objIndex = 0 );
		//void				SetTime( float _time, UINT objIndex = 0 );
		//float				GetTime( UINT objIndex = 0 );

		void				Update();
		void				Draw();
	};
}}
