#include "GameWindow.h"
#include "Input.h"
#include "Renderer.h"

namespace Sentinel
{
	Renderer* gRenderer = NULL;

	GameWindow::GameWindow( UINT icon, UINT iconSmall, UINT menu, LPCSTR cursor )
	{
		mIcon		= icon;
		mIconSmall	= iconSmall;
		mMenu		= menu;
		mCursor		= cursor;

		memset( mTitle, 0, MAX_TITLE_LENGTH );
		memset( mWindowClass, 0, MAX_CLASS_LENGTH );

		mWindowInfo = NULL;
		mRenderer   = NULL;
	}

	GameWindow::~GameWindow()
	{}

	void GameWindow::Startup( Renderer* renderer, HINSTANCE hInstance, int nCmdShow, TCHAR* title, TCHAR* windowClass, const WindowInfo& info )
	{
		strcpy_s( mTitle, title );
		strcpy_s( mWindowClass, windowClass );
		
		mINST = hInstance;

		HWND hWnd;

		if( RegisterWindowClass() )
		{
			RECT rect = { 0, 0, info.Width(), info.Height() };
			DWORD exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			DWORD dwStyle = (!info.Fullscreen()) ? WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN : WS_POPUP;
			AdjustWindowRectEx( &rect, dwStyle, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE );

			hWnd = CreateWindowEx( exStyle, mWindowClass, mTitle, 
								   dwStyle,
								   0, 0, rect.right-rect.left, rect.bottom-rect.top,
								   NULL, NULL, mINST, NULL );

			if( !hWnd )
				throw AppException( "Failed to initialize window: " + std::string( mTitle ));

			//SetWindowLong(hWnd, GWL_STYLE, 0);	// borderless window

			ShowWindow( hWnd, nCmdShow );
			UpdateWindow( hWnd );

			mWindowInfo = renderer->Startup( hWnd, info.Fullscreen(), info.Width(), info.Height() );

			if( !mWindowInfo )
				throw AppException( "Failed Renderer::Startup()" );

			mRenderer = renderer;
			gRenderer = renderer;
		}
		else
		{
			throw AppException( "Failed GameWindow::Startup()" );
		}
	}

	void GameWindow::Update()
	{
		gRenderer = mRenderer;

		mWindowInfo->Update();
	}

	void GameWindow::Shutdown()
	{
		mRenderer->SetWindow( mWindowInfo );
		mRenderer->Shutdown();

		UnregisterClass( mWindowClass, mINST );

		delete mWindowInfo;
		mWindowInfo = NULL;
	}

	///////////////////////////

	const WindowInfo* GameWindow::GetInfo() const
	{
		return mWindowInfo;
	}

	///////////////////////////

	LRESULT CALLBACK GameWindow::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		Mouse::Get().ProcessMessages( hWnd, msg, wParam, lParam );
		
		switch( msg )
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SIZE:
			gRenderer->ResizeBuffers( LOWORD( lParam ), HIWORD( lParam ));
			break;
		}

		return DefWindowProc( hWnd, msg, wParam, lParam );
	}

	bool GameWindow::RegisterWindowClass()
	{
		WNDCLASSEX wcex;

		if( GetClassInfoEx( mINST, (LPCSTR)mWindowClass, &wcex ))
			return true;

		wcex.cbSize			= sizeof( WNDCLASSEX );
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= mINST;
		wcex.hIcon			= LoadIcon( wcex.hInstance, MAKEINTRESOURCE( mIcon ));
		wcex.hIconSm		= LoadIcon( wcex.hInstance, MAKEINTRESOURCE( mIconSmall ));
		wcex.lpszMenuName	= MAKEINTRESOURCE( mMenu );
		wcex.hCursor		= LoadCursor( NULL, mCursor );
		wcex.hbrBackground	= (HBRUSH)( COLOR_WINDOW+1 );
		wcex.lpszClassName	= mWindowClass;

		if( !RegisterClassEx( &wcex ))
		{
			TRACE( "Error: RegisterClass = " << GetLastError() );
			return false;
		}
		
		return true;
	}
}
