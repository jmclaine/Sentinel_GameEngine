#include "WShader.h"

namespace Sentinel { namespace Assets
{
	WShader::WShader()
	{}

	WShader::WShader( WShader^ shader )
	{
		mRef = shader->mRef;
	}

	WShader::WShader( Shader* shader )
	{
		mRef = shader;
	}

	WShader::~WShader()
	{}

	Shader* WShader::GetRef()
	{
		return mRef;
	}

	System::String^	WShader::AttributeDecl()
	{
		return gcnew System::String( mRef->AttributeDecl().c_str() );
	}

	System::String^	WShader::UniformDecl()
	{
		return gcnew System::String( mRef->UniformDecl().c_str() );
	}

	UINT WShader::VertexSize()
	{
		return mRef->VertexSize();
	}

	void WShader::Release()
	{
		mRef->Release();
	}

	void WShader::ApplyPass()
	{
		mRef->ApplyPass();
	}

	void WShader::ApplyLayout()
	{
		mRef->ApplyLayout();
	}

	void WShader::SetFloat( UINT uniform, float data )
	{
		mRef->SetFloat( uniform, data );
	}

	void WShader::SetFloat2( UINT uniform, float* data, UINT offset, UINT count )
	{
		mRef->SetFloat2( uniform, data, offset, count );
	}

	void WShader::SetFloat3( UINT uniform, float* data, UINT offset, UINT count )
	{
		mRef->SetFloat3( uniform, data, offset, count );
	}

	void WShader::SetFloat4( UINT uniform, float* data, UINT offset, UINT count )
	{
		mRef->SetFloat4( uniform, data, offset, count );
	}

	void WShader::SetMatrix( UINT uniform, float* matrix, UINT offset, UINT count )
	{
		mRef->SetMatrix( uniform, matrix, offset, count );
	}

	void WShader::SetTexture( UINT uniform, WTexture^ texture )
	{
		mRef->SetTexture( uniform, texture->GetRef() );
	}
}}
