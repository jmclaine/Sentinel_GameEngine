#include "WRenderer.h"
#include "Renderer.h"
#include "SString.h"

namespace Sentinel { namespace Systems
{
	LRESULT WINAPI RendererMsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		switch( msg )
		{
			case WM_IME_SETCONTEXT:
				if( LOWORD( wParam ) > 0 )
					SetFocus( hWnd );

				return 0;

			default:
				return DefWindowProc( hWnd, msg, wParam, lParam );
		}
	}

	WRenderer::WRenderer()
	{}

	WRenderer::~WRenderer()
	{}

	bool WRenderer::Load( System::String^ filename )
	{
		// Somehow the pointer created in the DLL SingletonAbstract
		// is not the same as the SingletonAbstract located here,
		// which is nullptr.
		//
		if( !Renderer::Inst( (Renderer*)(Renderer::Load( SString( filename )))))
			return false;
		
		return true;
	}

	UINT WRenderer::Startup( IntPtr hWnd )
	{
		return Renderer::Inst()->Startup( hWnd.ToPointer() );
	}

	void WRenderer::Update( Object^ sender, EventArgs^ e )
	{
		Renderer::Inst()->Clear( ColorRGBA( 0.0f, 0.2f, 0.8f, 1.0f ).Ptr() );

		// Draw stuff.

		Renderer::Inst()->Present();
	}

	void WRenderer::Destroy()
	{
		Renderer::Destroy();
	}

	// Buffers.
	//
	WBuffer^ WRenderer::CreateBuffer( IntPtr data, UINT size, UINT stride, BufferType type )
	{
		return gcnew WBuffer( Renderer::Inst()->CreateBuffer( data.ToPointer(), size, stride, type ));
	}

	void WRenderer::SetVBO( WBuffer^ buffer )
	{
		Renderer::Inst()->SetVBO( buffer->GetRef() );
	}

	void WRenderer::SetIBO( WBuffer^ buffer )
	{
		Renderer::Inst()->SetIBO( buffer->GetRef() );
	}

	// Textures.
	//
	WTexture^ WRenderer::CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return gcnew WTexture( Renderer::Inst()->CreateTexture( width, height, (Sentinel::ImageFormatType)format, createMips ));
	}

	WTexture^ WRenderer::CreateTextureFromFile( String^ filename )
	{
		return gcnew WTexture( Renderer::Inst()->CreateTextureFromFile( SString( filename )));
	}

	WTexture^ WRenderer::CreateTextureFromMemory( IntPtr data, UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return gcnew WTexture( Renderer::Inst()->CreateTextureFromMemory( data.ToPointer(), width, height, (Sentinel::ImageFormatType)format, createMips ));
	}
	
	// Special Rendering.
	//
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

	void WRenderer::SetDepthStencilState( UINT state )
	{
		Renderer::Inst()->SetDepthStencilState( state );
	}

	void WRenderer::SetViewport( UINT viewport )
	{
		Renderer::Inst()->SetViewport( viewport );
	}

	UINT WRenderer::SetCull( CullType type )
	{
		return Renderer::Inst()->SetCull( (Sentinel::CullType)type );
	}

	void WRenderer::SetBlend( BlendType type )
	{
		Renderer::Inst()->SetBlend( (Sentinel::BlendType)type );
	}

	// Shaders.
	//
	WShader^ WRenderer::CreateShader( System::String^ filename, System::String^ attrib, System::String^ uniform )
	{
		return gcnew WShader( Renderer::Inst()->CreateShader( SString( filename ), SString( attrib ), SString( uniform )));
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

	IntPtr WRenderer::WndProc( IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled )
	{
		switch( msg )
		{
			case WM_IME_SETCONTEXT:
				if( LOWORD( wParam.ToInt32() ) > 0)
					SetFocus( mHWND );

				handled = true;
				return IntPtr::Zero;
		}

		handled = false;

		return IntPtr::Zero;
	}

	void WRenderer::OnRenderSizeChanged( SizeChangedInfo^ sizeInfo )
	{
		Renderer::Inst()->ResizeBuffers( (UINT)sizeInfo->NewSize.Width, (UINT)sizeInfo->NewSize.Height );
	}

	void WRenderer::OnRender( DrawingContext^ drawingContext )
	{
		// Better to do nothing as the rendering
		// should be taking place in app.
	}

	void WRenderer::OnMouseDown( MouseButtonEventArgs^ e )
	{}

	void WRenderer::DestroyWindowCore( HandleRef hwnd )
	{
		Renderer::Destroy();

		if( mHWND != NULL && mHWND == (HWND)hwnd.Handle.ToPointer())
		{
			::DestroyWindow( mHWND );
			mHWND = NULL;
		}

		UnregisterClass( (LPCSTR)mWindowClass, mINST );
	}

	bool WRenderer::RegisterWindowClass()
	{
		WNDCLASS wndClass;

		if( GetClassInfo( mINST, (LPCSTR)mWindowClass, &wndClass ))
			return true;
		
		wndClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wndClass.lpfnWndProc	= (WNDPROC)RendererMsgProc;
		wndClass.cbClsExtra		= 0;
		wndClass.cbWndExtra		= 0;
		wndClass.hInstance		= mINST;
		wndClass.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
		wndClass.hCursor		= LoadCursor(0, IDC_ARROW);
		wndClass.hbrBackground	= 0;
		wndClass.lpszMenuName	= 0;
		wndClass.lpszClassName	= (LPCSTR)mWindowClass;

		if( !RegisterClass( &wndClass ))
		{
			TRACE( "Error: RegisterClass = " << GetLastError() );
			return false;
		}
		
		return true;
	}

	HandleRef WRenderer::BuildWindowCore( HandleRef hwndParent )
	{
		mINST			= (HINSTANCE)GetModuleHandle( NULL );
		mTitle			= L"Renderer";
		mWindowClass	= L"RendererClass";
		
		if( RegisterWindowClass() )
		{
			HWND hWnd = (HWND)hwndParent.Handle.ToPointer();
			RECT screenRect;
			GetWindowRect( hWnd, &screenRect );

			Renderer::Inst()->WINDOW_WIDTH  = screenRect.right  - screenRect.left;
			Renderer::Inst()->WINDOW_HEIGHT = screenRect.bottom - screenRect.top;
			
			mHWND = CreateWindowEx( 0, (LPCSTR)mWindowClass, (LPCSTR)mTitle, WS_CHILD | WS_VISIBLE, 
									0, 0, Renderer::Inst()->WINDOW_WIDTH, Renderer::Inst()->WINDOW_HEIGHT,
									hWnd, 0, mINST, 0 );

			if( !mHWND )
				TRACE( "Error: CreateWindowEx = " << GetLastError() );
			
			Renderer::Inst()->Startup( mHWND );

			return HandleRef( this, IntPtr( mHWND ));
		}

		return HandleRef( nullptr, IntPtr::Zero );
	}
}}
