#pragma once
/*
Shaders are created from WRenderer.CreateShader()
*/
#include "Property.h"
#include "Shader.h"
#include "m_shared_ptr.h"

namespace Sentinel { namespace Wrapped
{
	ref class WTexture;

	public ref class WShader
	{
		DECLARE_REF_SHARED( Shader );

	public:

		DECLARE_OP_SHARED( Shader );

		System::String^	Attribute();
		System::String^	Uniform();

		UINT  			VertexSize();

		void			ApplyPass();
		void			ApplyLayout();

		void			SetFloat( UINT uniform, float data );
		void			SetFloat2( UINT uniform, float* data, UINT offset, UINT count );
		void			SetFloat3( UINT uniform, float* data, UINT offset, UINT count );
		void			SetFloat4( UINT uniform, float* data, UINT offset, UINT count );
		void			SetMatrix( UINT uniform, float* matrix, UINT offset, UINT count );
		void			SetTexture( UINT uniform, WTexture^ texture );
	};

	DECLARE_CLASS_REF_SHARED( Shader );
}}
