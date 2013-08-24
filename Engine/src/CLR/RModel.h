#pragma once

#include "m_shared_ptr.h"
#include "Model.h"
#include "WShader.h"
#include "WMatrix4f.h"
#include "WMaterial.h"

using namespace Sentinel::Math;

namespace Sentinel { namespace Assets
{
	public ref class RModel
	{
	private:

		m_shared_ptr< Model > mRef;

	public:

		static WShader^		SHADER_COLOR;
		static WShader^		SHADER_TEXTURE;
		static WShader^		SHADER_NORMAL_MAP;
		static WShader^		SHADER_PARALLAX;
		static WShader^		SHADER_SKINNING;

		RModel( Model* model );
		RModel( std::shared_ptr< Model > model );
		~RModel();

		m_shared_ptr< Model > GetRef();

		////////////////////////////////

		RMatrix4f^			MatrixWorld();

		////////////////////////////////

		static RModel^		Load( System::String^ filename );

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
