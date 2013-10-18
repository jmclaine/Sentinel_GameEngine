#include "WShader.h"
#include "WTexture.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_SHARED( Shader );

	DEFINE_OP_SHARED( Shader );

	////////////////////////////////

	System::String^	WShader::Attribute()
	{
		return gcnew System::String( mRef->Attribute().c_str() );
	}

	System::String^	WShader::Uniform()
	{
		return gcnew System::String( mRef->Uniform().c_str() );
	}

	UINT WShader::VertexSize()
	{
		return mRef->VertexSize();
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
		mRef->SetTexture( uniform, texture->GetRef().get() );
	}

	////////////////////////////////

	DEFINE_CLASS_REF_SHARED( Shader );
}}
