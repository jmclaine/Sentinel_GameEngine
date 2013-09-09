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

		// These member functions are necessary due to inability
		// to directly access the static variables within the
		// Model class.
		//
		static void			SetShaderColor( WShader^ shader );
		static WShader^		GetShaderColor();

		static void			SetShaderTexture( WShader^ shader );
		static WShader^		GetShaderTexture();

		static void			SetShaderNormalMap( WShader^ shader );
		static WShader^		GetShaderNormalMap();

		static void			SetShaderParallax( WShader^ shader );
		static WShader^		GetShaderParallax();

		static void			SetShaderSkinning( WShader^ shader );
		static WShader^		GetShaderSkinning();

		bool				Create( System::String^ filename );
		void				Release();

		void				SetMaterials( WStdVector_Material^ material );
		void				GetMaterials( WStdVector_Material^ material );

		//void				SetKeyFrame( const KeyFrame& key, int keyIndex = -1, int objIndex = 0 );
		//void				SetTime( float _time, UINT objIndex = 0 );
		//float				GetTime( UINT objIndex = 0 );

		void				Update();
		void				Draw();
	};
}}
