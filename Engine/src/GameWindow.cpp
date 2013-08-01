#include "GameWindow.h"
#include "Input.h"

namespace Sentinel
{
	GameWindow::GameWindow( UINT icon, UINT iconSmall, UINT menu, LPCSTR cursor )
	{
		mHWND		= NULL;
		
		mIcon		= icon;
		mIconSmall	= iconSmall;
		mMenu		= menu;
		mCursor		= cursor;
	}

	GameWindow::~GameWindow()
	{}

	void GameWindow::Startup( HINSTANCE hInstance, int nCmdShow, TCHAR* title, TCHAR* windowClass, const Renderer::WindowInfo& info )
	{
		strcpy_s( mTitle, title );
		strcpy_s( mWindowClass, windowClass );
		
		mINST = hInstance;

		if( RegisterWindowClass() )
		{
			mHWND = CreateWindow( mWindowClass, mTitle, (!info.mFullscreen) ? WS_OVERLAPPEDWINDOW : WS_POPUP,
								  0, 0, info.mWidth, info.mHeight,
								  NULL, NULL, mINST, NULL );

			if( !mHWND )
				throw AppException( "Failed to initialize window: " + std::string( mTitle ));
			
			ShowWindow( mHWND, nCmdShow );
			UpdateWindow( mHWND );
		}

		mWindowInfo = Renderer::Inst()->Startup( mHWND, info.mFullscreen, info.mWidth, info.mHeight );

		if( !mWindowInfo )
			throw AppException( "Failed Renderer::Startup()" );

		mDepthStencil	= Renderer::Inst()->CreateDepthStencil( mWindowInfo->mWidth, mWindowInfo->mHeight );
		mViewport		= Renderer::Inst()->CreateViewport( mWindowInfo->mWidth, mWindowInfo->mHeight );
		mRenderTarget	= Renderer::Inst()->CreateBackbuffer();
	}

	void GameWindow::Update()
	{
		static float color[] = {0.0f, 0.2f, 0.8f, 1.0f};

		Renderer::Inst()->SetWindow( mWindowInfo );

		Renderer::Inst()->SetDepthStencil( mDepthStencil );
		Renderer::Inst()->SetViewport( mViewport );
		Renderer::Inst()->SetRenderTarget( mRenderTarget );

		Renderer::Inst()->Clear( color );
	}

	void GameWindow::Shutdown()
	{
		SetActive();

		Renderer::Inst()->Shutdown();

		UnregisterClass( mWindowClass, mINST );
	}

	///////////////////////////

	void GameWindow::SetActive()
	{
		Renderer::Inst()->SetWindow( mWindowInfo );
	}

	bool GameWindow::ShareResources( GameWindow* window )
	{
		return Renderer::Inst()->ShareResources( mWindowInfo, window->mWindowInfo );
	}

	HWND GameWindow::GetHandle()
	{
		return mHWND;
	}

	Renderer::WindowInfo* GameWindow::GetWindow()
	{
		return mWindowInfo;
	}

	///////////////////////////

	LRESULT CALLBACK GameWindow::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		Mouse::Inst()->ProcessMessages( message );
		Keyboard::Inst()->ProcessMessages();

		switch( message )
		{
			case WM_COMMAND:
				return DefWindowProc( hWnd, message, wParam, lParam );
			
			case WM_PAINT:
				{
				PAINTSTRUCT ps;
				BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
				}
				break;

			case WM_DESTROY:
				PostQuitMessage( 0 );
				break;
		}

		return DefWindowProc( hWnd, message, wParam, lParam );
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
