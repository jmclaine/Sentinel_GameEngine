#pragma once

#include "Shader.h"
#include "Texturew.h"

namespace Sentinel { namespace Assets
{
	public ref class Shaderw
	{
	private:

		Shader*				mRef;

		Shaderw();

	public:

		Shaderw( Shaderw^ shader );
		Shaderw( Shader* shader );
		~Shaderw();

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
		void				SetTexture( UINT uniform, Texturew^ texture );
	};
}}
