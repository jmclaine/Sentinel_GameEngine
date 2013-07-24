#include "Rendererw.h"
#include "Renderer.h"
#include "SString.h"

namespace Sentinel { namespace Systems
{
	Rendererw::~Rendererw()
	{}

	bool Rendererw::Load( System::String^ filename )
	{
		// Somehow the pointer created in the DLL SingletonAbstract
		// is not the same as the SingletonAbstract located here.
		//
		if( !Renderer::Inst( const_cast< Renderer* >(Renderer::Load( SString( filename )))))
			return false;
		
		return true;
	}

	UINT Rendererw::Startup( System::IntPtr hWnd, bool fullscreen, UINT width, UINT height )
	{
		return Renderer::Inst()->Startup( (HWND)hWnd.ToPointer(), fullscreen, width, height );
	}

	void Rendererw::Destroy()
	{
		Renderer::Destroy();
	}

	// Buffers.
	//
	Bufferw^ Rendererw::CreateBuffer( System::IntPtr data, UINT size, UINT stride, BufferType type )
	{
		return gcnew Bufferw( Renderer::Inst()->CreateBuffer( data.ToPointer(), size, stride, type ));
	}

	void Rendererw::SetVBO( Bufferw^ buffer )
	{
		Renderer::Inst()->SetVBO( buffer->GetRef() );
	}

	void Rendererw::SetIBO( Bufferw^ buffer )
	{
		Renderer::Inst()->SetIBO( buffer->GetRef() );
	}

	// Textures.
	//
	Texturew^ Rendererw::CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return gcnew Texturew( Renderer::Inst()->CreateTexture( width, height, (Sentinel::ImageFormatType)format, createMips ));
	}

	Texturew^ Rendererw::CreateTextureFromFile( System::String^ filename )
	{
		return gcnew Texturew( Renderer::Inst()->CreateTextureFromFile( SString( filename )));
	}

	Texturew^ Rendererw::CreateTextureFromMemory( System::IntPtr data, UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return gcnew Texturew( Renderer::Inst()->CreateTextureFromMemory( data.ToPointer(), width, height, (Sentinel::ImageFormatType)format, createMips ));
	}
	
	// Special Rendering.
	//
	UINT Rendererw::CreateRenderTarget( Texturew^ texture )
	{
		return Renderer::Inst()->CreateRenderTarget( texture->GetRef() );
	}

	UINT Rendererw::CreateDepthStencil( UINT width, UINT height )
	{
		return Renderer::Inst()->CreateDepthStencil( width, height );
	}

	UINT Rendererw::CreateViewport( UINT width, UINT height )
	{
		return Renderer::Inst()->CreateViewport( width, height );
	}

	UINT Rendererw::ResizeBuffers( UINT width, UINT height )
	{
		return Renderer::Inst()->ResizeBuffers( width, height );
	}

	void Rendererw::SetRenderType( PrimitiveType type )
	{
		Renderer::Inst()->SetRenderType( (Sentinel::PrimitiveType)type );
	}

	void Rendererw::SetRenderTarget( UINT target )
	{
		Renderer::Inst()->SetRenderTarget( target );
	}

	void Rendererw::SetDepthStencil( UINT stencil )
	{
		Renderer::Inst()->SetDepthStencil( stencil );
	}

	void Rendererw::SetDepthStencilState( UINT state )
	{
		Renderer::Inst()->SetDepthStencilState( state );
	}

	void Rendererw::SetViewport( UINT viewport )
	{
		Renderer::Inst()->SetViewport( viewport );
	}

	UINT Rendererw::SetCull( CullType type )
	{
		return Renderer::Inst()->SetCull( (Sentinel::CullType)type );
	}

	void Rendererw::SetBlend( BlendType type )
	{
		Renderer::Inst()->SetBlend( (Sentinel::BlendType)type );
	}

	// Shaders.
	//
	Shaderw^ Rendererw::CreateShader( System::String^ filename, System::String^ attrib, System::String^ uniform )
	{
		return gcnew Shaderw( Renderer::Inst()->CreateShader( SString( filename ), SString( attrib ), SString( uniform )));
	}

	void Rendererw::SetShader( Shaderw^ shader )
	{
		Renderer::Inst()->SetShader( shader->GetRef() );
	}

	// Rendering.
	//
	void Rendererw::Clear( ColorRGBAw^ color )
	{
		Renderer::Inst()->Clear( color->GetRef()->Ptr() );
	}

	void Rendererw::DrawIndexed( UINT count, UINT startIndex, UINT baseVertex )
	{
		Renderer::Inst()->DrawIndexed( count, startIndex, baseVertex );
	}

	void Rendererw::Present()
	{
		Renderer::Inst()->Present();
	}
}}
