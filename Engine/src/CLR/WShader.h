#pragma once
/*
Shaders are created from WRenderer.CreateShader()

Delete() does nothing in RShader.
*/
#include "m_shared_ptr.h"
#include "Shader.h"
#include "WTexture.h"

using namespace System;

namespace Sentinel { namespace Assets
{
	public ref class WShader
	{
	protected:

		Shader*			mRef;

		////////////////////////////////

		WShader();

	public:

		WShader( Shader* shader );
		~WShader();
		!WShader();

	protected:

		virtual void	Delete();

	public:

		Shader*			GetRef();

		////////////////////////////////

		String^			AttributeDecl();
		String^			UniformDecl();

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

	public ref class RShader : public WShader
	{
	private:

		Shader*&		mRefPtr;

	public:

		RShader( Shader*& shader );
		
		void			Set( Shader*& shader );
		void			Set( WShader^ shader );

	protected:

		virtual void	Delete() override;
	};
}}
