#include "WGameWindow.h"
#include "WInput.h"

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

	void WGameWindow::Update()
	{}

	void WGameWindow::Shutdown()
	{
		SetActive();

		Renderer::Inst()->Shutdown();
	}

	///////////////////////////

	// Mouse Down.
	//
	void WGameWindow::OnLeftMouseDown()
	{}

	void WGameWindow::OnRightMouseDown()
	{}

	void WGameWindow::OnMiddleMouseDown()
	{}

	// Mouse Up.
	//
	void WGameWindow::OnLeftMouseUp()
	{}

	void WGameWindow::OnRightMouseUp()
	{}

	void WGameWindow::OnMiddleMouseUp()
	{}

	// Mouse Double Click.
	//
	void WGameWindow::OnLeftMouseDoubleClick()
	{}

	void WGameWindow::OnRightMouseDoubleClick()
	{}

	void WGameWindow::OnMiddleMouseDoubleClick()
	{}

	// Scrolling.
	//
	void WGameWindow::OnMouseScroll()
	{}

	// Mouse Move.
	//
	void WGameWindow::OnMouseMove()
	{}

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

#define RETURN_HANDLE( isHandled )\
	handled = isHandled;\
	return IntPtr::Zero;

	IntPtr WGameWindow::WndProc( IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled )
	{
		handled = WMouse::ProcessMessages( hWnd, msg, wParam, lParam );

		switch( msg )
		{
			case WM_IME_SETCONTEXT:
				if( LOWORD( wParam.ToInt32() ) > 0)
					SetFocus( (HWND)hWnd.ToPointer() );
				RETURN_HANDLE( true );

			// Moving.
			//
			case WM_MOUSEMOVE:
				SetFocus( (HWND)hWnd.ToPointer() );
				OnMouseMove();
				RETURN_HANDLE( true );

			// Scrolling.
			//
			case WM_MOUSEWHEEL:
				OnMouseScroll();
				RETURN_HANDLE( true );
		}

		// Button Down.
		//
		if( WMouse::DidGoDown( BUTTON_LEFT ))
			OnLeftMouseDown();

		if( WMouse::DidGoDown( BUTTON_RIGHT ))
			OnRightMouseDown();

		if( WMouse::DidGoDown( BUTTON_MIDDLE ))
			OnMiddleMouseDown();

		// Button Up.
		//
		if( WMouse::DidGoUp( BUTTON_LEFT ))
			OnLeftMouseUp();

		if( WMouse::DidGoUp( BUTTON_RIGHT ))
			OnRightMouseUp();

		if( WMouse::DidGoUp( BUTTON_MIDDLE ))
			OnMiddleMouseUp();

		// Button Double Click.
		//
		if( WMouse::DidDoubleClick( BUTTON_LEFT ))
			OnLeftMouseDoubleClick();

		if( WMouse::DidDoubleClick( BUTTON_RIGHT ))
			OnRightMouseDoubleClick();

		if( WMouse::DidDoubleClick( BUTTON_MIDDLE ))
			OnMiddleMouseDoubleClick();

		if( handled )
			return IntPtr::Zero;

		RETURN_HANDLE( false );
	}

	LRESULT WINAPI GameWindowMsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
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
		HwndHost::OnMouseDown( e );
	}

	bool WGameWindow::RegisterWindowClass()
	{
		WNDCLASS wndClass;

		if( GetClassInfo( mINST, (LPCSTR)mWindowClass, &wndClass ))
			return true;
		
		wndClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wndClass.lpfnWndProc	= (WNDPROC)GameWindowMsgProc;
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
			mHWND = CreateWindowEx( 0, (LPCSTR)mWindowClass, (LPCSTR)mTitle, WS_CHILD | WS_VISIBLE | DS_CONTROL, 
									0, 0, mWindowInfo->Width(), mWindowInfo->Height(),
									(HWND)hwndParent.Handle.ToPointer(), 0, mINST, 0 );

			if( !mHWND )
				TRACE( "Error: CreateWindowEx = " << GetLastError() );
			
			mWindowInfo = gcnew WWindowInfo( Renderer::Inst()->Startup( mHWND, mWindowInfo->Fullscreen(), mWindowInfo->Width(), mWindowInfo->Height() ));

			// Create default settings.
			//
			WRenderer::CreateDepthStencil( mWindowInfo->Width(), mWindowInfo->Height() );
			WRenderer::CreateViewport( mWindowInfo->Width(), mWindowInfo->Height() );
			WRenderer::CreateBackbuffer();
			
			return HandleRef( this, IntPtr( mHWND ));
		}

		return HandleRef( nullptr, IntPtr::Zero );
	}

	void WGameWindow::DestroyWindowCore( HandleRef hwnd )
	{
		if( mHWND != NULL && mHWND == (HWND)hwnd.Handle.ToPointer())
		{
			::DestroyWindow( mHWND );
			mHWND = NULL;
		}

		UnregisterClass( (LPCSTR)mWindowClass, mINST );
	}
}}
