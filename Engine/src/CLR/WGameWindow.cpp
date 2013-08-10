#include "WGameWindow.h"

namespace Sentinel { namespace Systems
{
	WGameWindow::WGameWindow()
	{
		mTitle		 = NULL;
		mWindowClass = NULL;
	}

	WGameWindow::~WGameWindow()
	{
		delete mTitle;
		delete mWindowClass;
	}

	void WGameWindow::Startup( String^ title, String^ windowClass, WWindowInfo^ info )
	{
		_ASSERT( title->Length < MAX_TITLE_LENGTH && windowClass->Length < MAX_CLASS_LENGTH );

		mTitle		 = new wchar_t[ MAX_TITLE_LENGTH ];
		mWindowClass = new wchar_t[ MAX_CLASS_LENGTH ];

		memset( mTitle, 0, MAX_TITLE_LENGTH );
		memset( mWindowClass, 0, MAX_CLASS_LENGTH );

		for( int x = 0; x < title->Length; ++x )
			mTitle[ x ] = title[ x ];

		for( int x = 0; x < windowClass->Length; ++x )
			mWindowClass[ x ] = windowClass[ x ];

		mWindowInfo = info;
	}

	void WGameWindow::Shutdown()
	{
		SetActive();

		Renderer::Inst()->Shutdown();
	}

	///////////////////////////

	void WGameWindow::SetActive()
	{
		WRenderer::SetWindow( mWindowInfo );
	}

	bool WGameWindow::ShareResources( WGameWindow^ window )
	{
		return WRenderer::ShareResources( mWindowInfo, window->GetInfo() );
	}

	WWindowInfo^ WGameWindow::GetInfo()
	{
		return mWindowInfo;
	}

	///////////////////////////

	IntPtr WGameWindow::WndProc( IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled )
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

	void WGameWindow::OnRenderSizeChanged( SizeChangedInfo^ sizeInfo )
	{
		Renderer::Inst()->ResizeBuffers( (UINT)sizeInfo->NewSize.Width, (UINT)sizeInfo->NewSize.Height );
	}

	void WGameWindow::OnRender( DrawingContext^ drawingContext )
	{
		// Better to do nothing as the rendering should be taking place in app.
	}

	void WGameWindow::OnMouseDown( MouseButtonEventArgs^ e )
	{
		// Handled within app.
	}

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

	bool WGameWindow::RegisterWindowClass()
	{
		WNDCLASS wndClass;

		if( GetClassInfo( mINST, (LPCSTR)mWindowClass, &wndClass ))
			return true;
		
		wndClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wndClass.lpfnWndProc	= (WNDPROC)RendererMsgProc;
		wndClass.cbClsExtra		= 0;
		wndClass.cbWndExtra		= 0;
		wndClass.hInstance		= mINST;
		wndClass.hIcon			= LoadIcon( NULL, IDI_WINLOGO );
		wndClass.hCursor		= LoadCursor( 0, IDC_ARROW );
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

	HandleRef WGameWindow::BuildWindowCore( HandleRef hwndParent )
	{
		mINST = (HINSTANCE)GetModuleHandle( NULL );
		
		if( RegisterWindowClass() )
		{
			mHWND = CreateWindowEx( 0, (LPCSTR)mWindowClass, (LPCSTR)mTitle, WS_CHILD | WS_VISIBLE, 
									0, 0, mWindowInfo->GetWidth(), mWindowInfo->GetHeight(),
									(HWND)hwndParent.Handle.ToPointer(), 0, mINST, 0 );

			if( !mHWND )
				TRACE( "Error: CreateWindowEx = " << GetLastError() );
			
			mWindowInfo = gcnew WWindowInfo( Renderer::Inst()->Startup( mHWND, mWindowInfo->GetFullscreen(), mWindowInfo->GetWidth(), mWindowInfo->GetHeight() ));

			// Create default (0) settings.
			//
			WRenderer::CreateDepthStencil( mWindowInfo->GetWidth(), mWindowInfo->GetHeight() );
			WRenderer::CreateViewport( mWindowInfo->GetWidth(), mWindowInfo->GetHeight() );
			WRenderer::CreateBackbuffer();
			
			return HandleRef( this, IntPtr( mHWND ));
		}

		return HandleRef( nullptr, IntPtr::Zero );
	}

	void WGameWindow::DestroyWindowCore( HandleRef hwnd )
	{
		SetActive();
		Shutdown();

		if( mHWND != NULL && mHWND == (HWND)hwnd.Handle.ToPointer())
		{
			::DestroyWindow( mHWND );
			mHWND = NULL;
		}

		UnregisterClass( (LPCSTR)mWindowClass, mINST );
	}
}}
