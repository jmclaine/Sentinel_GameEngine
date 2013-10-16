#include "GameWindow.h"
#include "Input.h"
#include "Renderer.h"

namespace Sentinel
{
	GameWindow::GameWindow( UINT icon, UINT iconSmall, UINT menu, LPCSTR cursor )
	{
		mHWND		= NULL;
		
		mIcon		= icon;
		mIconSmall	= iconSmall;
		mMenu		= menu;
		mCursor		= cursor;

		memset( mTitle, 0, MAX_TITLE_LENGTH );
		memset( mWindowClass, 0, MAX_CLASS_LENGTH );

		mWindowInfo = NULL;
	}

	GameWindow::~GameWindow()
	{}

	void GameWindow::Startup( Renderer* renderer, HINSTANCE hInstance, int nCmdShow, TCHAR* title, TCHAR* windowClass, const WindowInfo& info )
	{
		mRenderer = renderer;

		strcpy_s( mTitle, title );
		strcpy_s( mWindowClass, windowClass );
		
		mINST = hInstance;

		if( RegisterWindowClass() )
		{
			mHWND = CreateWindow( mWindowClass, mTitle, (!info.Fullscreen()) ? WS_OVERLAPPEDWINDOW : WS_POPUP,
								  0, 0, info.Width(), info.Height(),
								  NULL, NULL, mINST, NULL );

			if( !mHWND )
				throw AppException( "Failed to initialize window: " + std::string( mTitle ));
			
			ShowWindow( mHWND, nCmdShow );
			UpdateWindow( mHWND );
		}

		mWindowInfo = mRenderer->Startup( mHWND, info.Fullscreen(), info.Width(), info.Height() );

		if( !mWindowInfo )
			throw AppException( "Failed Renderer::Startup()" );
	}

	void GameWindow::Shutdown()
	{
		SetActive();

		mRenderer->Shutdown();

		UnregisterClass( mWindowClass, mINST );

		delete mWindowInfo;
		mWindowInfo = NULL;
	}

	///////////////////////////

	void GameWindow::SetActive()
	{
		mRenderer->SetWindow( mWindowInfo );
	}

	bool GameWindow::ShareResources( GameWindow* window )
	{
		return mRenderer->ShareResources( mWindowInfo, window->mWindowInfo );
	}

	HWND GameWindow::GetHandle()
	{
		return mHWND;
	}

	const WindowInfo* GameWindow::GetInfo() const
	{
		return mWindowInfo;
	}

	///////////////////////////

	LRESULT CALLBACK GameWindow::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		Mouse::Get().ProcessMessages( hWnd, msg, wParam, lParam );
		Keyboard::Get().ProcessMessages();

		switch( msg )
		{
			case WM_COMMAND:
				return DefWindowProc( hWnd, msg, wParam, lParam );
			
			case WM_PAINT:
				{
				PAINTSTRUCT ps;
				BeginPaint( hWnd, &ps );
				EndPaint( hWnd, &ps );
				}
				break;

			case WM_DESTROY:
				PostQuitMessage( 0 );
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
