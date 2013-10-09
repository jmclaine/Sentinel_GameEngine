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

		DECLARE_OP_SHARED( Model );

		DECLARE_PROPERTY( WMatrix4f^, MatrixWorld );

		////////////////////////////////

		static WModel^		Load( System::String^ filename );

		void				SetMaterials( WStdVector_Material^ material );
		void				GetMaterials( WStdVector_Material^ material );

		//void				SetTime( float _time, UINT objIndex = 0 );
		//float				GetTime( UINT objIndex = 0 );
		void				Update();

		void				Draw();
	};

	DECLARE_CLASS_REF_SHARED( Model );
}}
