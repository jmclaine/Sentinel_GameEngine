#include "WRenderer.h"
#include "Renderer.h"
#include "RString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Systems
{
	DEFINE_REF( WindowInfo );

	WWindowInfo::WWindowInfo()
	{
		mRef = new WindowInfo();
	}

	WWindowInfo::WWindowInfo( const WindowInfo% info )
	{
		mRef = new WindowInfo( info );
	}

	WWindowInfo::WWindowInfo( WindowInfo* info )
	{
		mRef = info;
	}

	WWindowInfo::WWindowInfo( const WWindowInfo% info )
	{
		mRef = new WindowInfo( *info.mRef );
	}

	bool WWindowInfo::Fullscreen()
	{
		return mRef->Fullscreen();
	}

	int WWindowInfo::Width()
	{
		return mRef->Width();
	}

	int WWindowInfo::Height()
	{
		return mRef->Height();
	}

	float WWindowInfo::WidthRatio()
	{
		return mRef->WidthRatio();
	}

	float WWindowInfo::HeightRatio()
	{
		return mRef->HeightRatio();
	}

	DEFINE_CLASS_REF( WindowInfo );

	/////////////////////////////////////////////////

	WWindowInfo^ WRenderer::Load( String^ filename )
	{
		// Somehow the pointer created in the DLL SingletonAbstract
		// is not the same as the SingletonAbstract located here,
		// which is nullptr.
		//
		WindowInfo info;

		bool result = (Renderer::Inst( (Renderer*)(Renderer::Load( RString::ToString( filename ).c_str(), info ))) != NULL);

		if( !result )
			return nullptr;

		return gcnew WWindowInfo( info );
	}

	void WRenderer::Shutdown()
	{
		Renderer::Inst()->Shutdown();
	}

	// Windows.
	//
	void WRenderer::SetWindow( WWindowInfo^ info )
	{
		Renderer::Inst()->SetWindow( info->GetRef() );
	}

	WWindowInfo^ WRenderer::GetWindow()
	{
		return gcnew RWindowInfo( Renderer::Inst()->GetWindow() );
	}

	bool WRenderer::ShareResources( WWindowInfo^ info0, WWindowInfo^ info1 )
	{
		return Renderer::Inst()->ShareResources( info0->GetRef(), info1->GetRef() );
	}

	// Buffers.
	//
	WBuffer^ WRenderer::CreateBuffer( IntPtr data, UINT size, UINT stride, BufferType type )
	{
		return gcnew WBuffer( Renderer::Inst()->CreateBuffer( data.ToPointer(), size, stride, type ));
	}

	void WRenderer::SetVBO( WBuffer^ buffer )
	{
		Renderer::Inst()->SetVBO( buffer );
	}

	void WRenderer::SetIBO( WBuffer^ buffer )
	{
		Renderer::Inst()->SetIBO( buffer );
	}

	// Textures.
	//
	WTexture^ WRenderer::CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return gcnew WTexture( Renderer::Inst()->CreateTexture( width, height, (Sentinel::ImageFormatType)format, createMips ));
	}

	WTexture^ WRenderer::CreateTextureFromFile( String^ filename )
	{
		return gcnew WTexture( Renderer::Inst()->CreateTextureFromFile( RString::ToString( filename ).c_str() ));
	}

	WTexture^ WRenderer::CreateTextureFromMemory( IntPtr data, UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return gcnew WTexture( Renderer::Inst()->CreateTextureFromMemory( data.ToPointer(), width, height, (Sentinel::ImageFormatType)format, createMips ));
	}
	
	// Special Rendering.
	//
	UINT WRenderer::CreateBackbuffer()
	{
		return Renderer::Inst()->CreateBackbuffer();
	}

	UINT WRenderer::CreateRenderTarget( WTexture^ texture )
	{
		return Renderer::Inst()->CreateRenderTarget( texture->GetRef() );
	}

	UINT WRenderer::CreateDepthStencil( UINT width, UINT height )
	{
		return Renderer::Inst()->CreateDepthStencil( width, height );
	}

	UINT WRenderer::CreateViewport( UINT width, UINT height )
	{
		return Renderer::Inst()->CreateViewport( width, height );
	}

	UINT WRenderer::ResizeBuffers( UINT width, UINT height )
	{
		return Renderer::Inst()->ResizeBuffers( width, height );
	}

	void WRenderer::SetRenderType( PrimitiveType type )
	{
		Renderer::Inst()->SetRenderType( (Sentinel::PrimitiveType)type );
	}

	void WRenderer::SetRenderTarget( UINT target )
	{
		Renderer::Inst()->SetRenderTarget( target );
	}

	void WRenderer::SetDepthStencil( UINT stencil )
	{
		Renderer::Inst()->SetDepthStencil( stencil );
	}

	void WRenderer::SetDepthStencilState( StencilType state )
	{
		Renderer::Inst()->SetDepthStencilState( (Sentinel::StencilType)state );
	}

	void WRenderer::SetViewport( UINT viewport )
	{
		Renderer::Inst()->SetViewport( viewport );
	}

	UINT WRenderer::SetCull( CullType type )
	{
		return Renderer::Inst()->SetCull( (Sentinel::CullType)type );
	}

	UINT WRenderer::SetFill( FillType type )
	{
		return Renderer::Inst()->SetFill( (Sentinel::FillType)type );
	}

	void WRenderer::SetBlend( BlendType type )
	{
		Renderer::Inst()->SetBlend( (Sentinel::BlendType)type );
	}

	// Shaders.
	//
	WShader^ WRenderer::CreateShader( String^ filename, String^ attrib, String^ uniform )
	{
		Shader* shader = Renderer::Inst()->CreateShader( RString::ToString( filename ).c_str(), RString::ToString( attrib ).c_str(), RString::ToString( uniform ).c_str() );

		return (shader) ? gcnew WShader( shader ) : nullptr;
	}

	void WRenderer::SetShader( WShader^ shader )
	{
		Renderer::Inst()->SetShader( shader->GetRef() );
	}

	// Rendering.
	//
	void WRenderer::Clear( WColorRGBA^ color )
	{
		Renderer::Inst()->Clear( color->GetRef()->Ptr() );
	}

	void WRenderer::DrawIndexed( UINT count, UINT startIndex, UINT baseVertex )
	{
		Renderer::Inst()->DrawIndexed( count, startIndex, baseVertex );
	}

	void WRenderer::Present()
	{
		Renderer::Inst()->Present();
	}
}}
