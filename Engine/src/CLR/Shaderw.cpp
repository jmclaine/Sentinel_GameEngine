#include "Shaderw.h"

namespace Sentinel { namespace Assets
{
	Shaderw::Shaderw()
	{}

	Shaderw::Shaderw( Shaderw^ shader )
	{
		mRef = shader->mRef;
	}

	Shaderw::Shaderw( Shader* shader )
	{
		mRef = shader;
	}

	Shaderw::~Shaderw()
	{}

	System::String^	Shaderw::AttributeDecl()
	{
		return gcnew System::String( mRef->AttributeDecl().c_str() );
	}

	System::String^	Shaderw::UniformDecl()
	{
		return gcnew System::String( mRef->UniformDecl().c_str() );
	}

	UINT Shaderw::VertexSize()
	{
		return mRef->VertexSize();
	}

	void Shaderw::Release()
	{
		mRef->Release();
	}

	void Shaderw::ApplyPass()
	{
		mRef->ApplyPass();
	}

	void Shaderw::ApplyLayout()
	{
		mRef->ApplyLayout();
	}

	void Shaderw::SetFloat( UINT uniform, float data )
	{
		mRef->SetFloat( uniform, data );
	}

	void Shaderw::SetFloat2( UINT uniform, float* data, UINT offset, UINT count )
	{
		mRef->SetFloat2( uniform, data, offset, count );
	}

	void Shaderw::SetFloat3( UINT uniform, float* data, UINT offset, UINT count )
	{
		mRef->SetFloat3( uniform, data, offset, count );
	}

	void Shaderw::SetFloat4( UINT uniform, float* data, UINT offset, UINT count )
	{
		mRef->SetFloat4( uniform, data, offset, count );
	}

	void Shaderw::SetMatrix( UINT uniform, float* matrix, UINT offset, UINT count )
	{
		mRef->SetMatrix( uniform, matrix, offset, count );
	}

	void Shaderw::SetTexture( UINT uniform, Texturew^ texture )
	{
		mRef->SetTexture( uniform, texture->GetRef() );
	}
}}
