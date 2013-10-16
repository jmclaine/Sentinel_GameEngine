#pragma once

#include "m_shared_ptr.h"
#include "Property.h"
#include "Model.h"

namespace Sentinel { namespace Wrapped
{
	ref class WShader;
	ref class WMatrix4f;
	ref class WMaterial;
	ref class WStdVector_Material;
	ref class WRenderer;
	ref class WShaderManager;
	ref class WTextureManager;
	ref class WGameWorld;

	public ref class WModel
	{
		DECLARE_REF_SHARED( Model );

	public:

		DECLARE_OP_SHARED( Model );

		DECLARE_PROPERTY( WMatrix4f^, MatrixWorld );

		////////////////////////////////

		static WModel^	Load( System::String^	filename,
							  WRenderer^		renderer, 
							  WShaderManager^	shaderManager, 
							  WTextureManager^	textureManager );

		void			SetMaterials( WStdVector_Material^ material );
		void			GetMaterials( WStdVector_Material^ material );

		//void			SetTime( float _time, UINT objIndex = 0 );
		//float			GetTime( UINT objIndex = 0 );
		void			Update( float DT );

		void			Draw( WRenderer^ renderer, WGameWorld^ world );
	};

	DECLARE_CLASS_REF_SHARED( Model );
}}
