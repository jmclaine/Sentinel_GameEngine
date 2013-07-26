#pragma once

#include "Shader.h"
#include "WTexture.h"

namespace Sentinel { namespace Assets
{
	public ref class WShader
	{
	private:

		Shader*				mRef;

		WShader();

	public:

		WShader( WShader^ shader );
		WShader( Shader* shader );
		~WShader();

		Shader*				GetRef();

		System::String^		AttributeDecl();
		System::String^		UniformDecl();

		UINT  				VertexSize();

		void				Release();

		void				ApplyPass();
		void				ApplyLayout();

		void				SetFloat( UINT uniform, float data );
		void				SetFloat2( UINT uniform, float* data, UINT offset, UINT count );
		void				SetFloat3( UINT uniform, float* data, UINT offset, UINT count );
		void				SetFloat4( UINT uniform, float* data, UINT offset, UINT count );
		void				SetMatrix( UINT uniform, float* matrix, UINT offset, UINT count );
		void				SetTexture( UINT uniform, WTexture^ texture );
	};
}}
