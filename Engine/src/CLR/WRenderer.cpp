#include "WRenderer.h"
#include "GameWindow.h"
#include "WTexture.h"
#include "WBuffer.h"
#include "WShader.h"
#include "WColorRGBA.h"
#include "WString.h"

using namespace Sentinel::Wrapped;

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF( WindowInfo );

	WWindowInfo::WWindowInfo()
	{
		mRef = new WindowInfo();
	}

	WWindowInfo::WWindowInfo( WindowInfo* info )
	{
		mRef = info;
	}

	//////////////////////////////

	DEFINE_OP_PTR( WindowInfo );

	bool WWindowInfo::Fullscreen()
	{
		return mRef->Fullscreen();
	}

	UINT WWindowInfo::Width()
	{
		return mRef->Width();
	}

	UINT WWindowInfo::Height()
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

	DEFINE_REF_PTR( Renderer );

	WRenderer::WRenderer( Renderer* renderer )
	{
		mRef = renderer;
	}

	void WRenderer::Release()
	{
		delete mRef;
	}

	WTexture^ WRenderer::NullTexture()
	{
		return gcnew WTexture( mRef->NULL_TEXTURE );
	}

	WTexture^ WRenderer::BaseTexture()
	{
		return gcnew WTexture( mRef->BASE_TEXTURE );
	}

	DEFINE_OP_PTR( Renderer );

	//////////////////////////////

	WRenderer^ WRenderer::Create( System::String^ filename, WWindowInfo^% info )
	{
		Renderer* renderer = Renderer::Create( msclr::interop::marshal_as< std::string >(filename).c_str(), *info->GetRef() );

		return (renderer) ? gcnew WRenderer( renderer ) : nullptr;
	}

	void WRenderer::Shutdown()
	{
		mRef->Shutdown();
	}

	// Windows.
	//
	void WRenderer::SetWindow( WWindowInfo^ info )
	{
		mRef->SetWindow( info->GetRef() );
	}

	WWindowInfo^ WRenderer::GetWindow()
	{
		return gcnew RWindowInfo( mRef->GetWindow() );
	}

	bool WRenderer::ShareResources( WWindowInfo^ info0, WWindowInfo^ info1 )
	{
		return mRef->ShareResources( info0->GetRef(), info1->GetRef() );
	}

	// Buffers.
	//
	WBuffer^ WRenderer::CreateBuffer( System::IntPtr data, UINT size, UINT stride, BufferType type )
	{
		return gcnew WBuffer( mRef->CreateBuffer( data.ToPointer(), size, stride, (Sentinel::BufferType)type ));
	}

	void WRenderer::SetVBO( WBuffer^ buffer )
	{
		mRef->SetVBO( buffer );
	}

	void WRenderer::SetIBO( WBuffer^ buffer )
	{
		mRef->SetIBO( buffer );
	}

	// Textures.
	//
	WTexture^ WRenderer::CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return gcnew WTexture( std::shared_ptr< Texture >( mRef->CreateTexture( width, height, (Sentinel::ImageFormatType)format, createMips )));
	}

	WTexture^ WRenderer::CreateTextureFromFile( System::String^ filename )
	{
		return gcnew WTexture( std::shared_ptr< Texture >( mRef->CreateTextureFromFile( WString::Alloc( filename ))));
	}

	WTexture^ WRenderer::CreateTextureFromMemory( System::IntPtr data, UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return gcnew WTexture( std::shared_ptr< Texture >( mRef->CreateTextureFromMemory( data.ToPointer(), width, height, (Sentinel::ImageFormatType)format, createMips )));
	}
	
	// Special Rendering.
	//
	UINT WRenderer::CreateBackbuffer()
	{
		return mRef->CreateBackbuffer();
	}

	UINT WRenderer::CreateRenderTarget( WTexture^ texture )
	{
		return mRef->CreateRenderTarget( texture );
	}

	UINT WRenderer::CreateDepthStencil( UINT width, UINT height )
	{
		return mRef->CreateDepthStencil( width, height );
	}

	UINT WRenderer::ResizeBuffers( UINT width, UINT height )
	{
		return mRef->ResizeBuffers( width, height );
	}

	void WRenderer::SetRenderType( PrimitiveType type )
	{
		mRef->SetRenderType( (Sentinel::PrimitiveType)type );
	}

	void WRenderer::SetRenderTarget( UINT target )
	{
		mRef->SetRenderTarget( target );
	}

	void WRenderer::SetDepthStencil( UINT stencil )
	{
		mRef->SetDepthStencil( stencil );
	}

	void WRenderer::SetDepthStencilState( StencilType state )
	{
		mRef->SetDepthStencilState( (Sentinel::StencilType)state );
	}

	void WRenderer::SetViewport( int x, int y, UINT width, UINT height )
	{
		mRef->SetViewport( x, y, width, height );
	}

	UINT WRenderer::SetCull( CullType type )
	{
		return mRef->SetCull( (Sentinel::CullType)type );
	}

	UINT WRenderer::SetFill( FillType type )
	{
		return mRef->SetFill( (Sentinel::FillType)type );
	}

	void WRenderer::SetBlend( BlendType type )
	{
		mRef->SetBlend( (Sentinel::BlendType)type );
	}

	// Shaders.
	//
	WShader^ WRenderer::CreateShaderFromFile( System::String^ filename, System::String^ attrib, System::String^ uniform )
	{
		std::shared_ptr< Shader > shader( mRef->CreateShaderFromFile( WString::Cast( filename ).c_str(), WString::Cast( attrib ).c_str(), WString::Cast( uniform ).c_str() ));

		return (shader) ? gcnew WShader( shader ) : nullptr;
	}

	void WRenderer::SetShader( WShader^ shader )
	{
		mRef->SetShader( shader );
	}

	// Rendering.
	//
	void WRenderer::Clear( WColorRGBA^ color )
	{
		mRef->Clear( color->GetRef()->Ptr() );
	}

	void WRenderer::DrawIndexed( UINT count, UINT startIndex, UINT baseVertex )
	{
		mRef->DrawIndexed( count, startIndex, baseVertex );
	}

	void WRenderer::Present()
	{
		mRef->Present();
	}

	WRenderer^ WRenderer::BuildGL()
	{
		return gcnew WRenderer( BuildRendererGL() );
	}

	WRenderer^ WRenderer::BuildDX()
	{
		return gcnew WRenderer( BuildRendererDX() );
	}
}}
