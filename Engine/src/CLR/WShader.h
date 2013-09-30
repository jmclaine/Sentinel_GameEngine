#pragma once
/*
Shaders are created from WRenderer.CreateShader()
*/
#include "WTexture.h"
#include "Shader.h"

namespace Sentinel { namespace Assets
{
	public ref class WShader
	{
		DECLARE_REF( Shader );

	protected:

		WShader();

	public:

		WShader( Shader* shader );

		////////////////////////////////

		DECLARE_OP_PTR( Shader );

		System::String^	Attribute();
		System::String^	Uniform();

		UINT  			VertexSize();

		void			Release();

		void			ApplyPass();
		void			ApplyLayout();

		void			SetFloat( UINT uniform, float data );
		void			SetFloat2( UINT uniform, float* data, UINT offset, UINT count );
		void			SetFloat3( UINT uniform, float* data, UINT offset, UINT count );
		void			SetFloat4( UINT uniform, float* data, UINT offset, UINT count );
		void			SetMatrix( UINT uniform, float* matrix, UINT offset, UINT count );
		void			SetTexture( UINT uniform, WTexture^ texture );
	};

	DECLARE_CLASS_REF( Shader );
	DECLARE_CLASS_REF_PTR( Shader );
}}
