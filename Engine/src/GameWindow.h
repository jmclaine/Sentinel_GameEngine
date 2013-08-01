#pragma once

#include <Windows.h>

#include "Renderer.h"

namespace Sentinel
{
	class SENTINEL_DLL GameWindow
	{
	public:

		static const UINT		MAX_LOADSTRING = 32;

	private:

		HWND					mHWND;
		HINSTANCE				mINST;

		UINT					mIcon;
		UINT					mIconSmall;
		UINT					mMenu;
		LPCSTR					mCursor;
		
		char					mTitle[ MAX_LOADSTRING ];
		char					mWindowClass[ MAX_LOADSTRING ];

		Renderer::WindowInfo*	mWindowInfo;

		UINT					mRenderTarget;
		UINT					mDepthStencil;
		UINT					mViewport;

	public:

		GameWindow( UINT icon, UINT iconSmall, UINT menu, LPCSTR cursor = IDC_ARROW );
		~GameWindow();

		void	Startup( HINSTANCE hInstance, int nCmdShow, char* title, char* windowClass, const Renderer::WindowInfo& info );

		void	Update();

		void	Shutdown();

		///////////////////////////

		void	SetActive();

		bool	ShareResources( GameWindow* window );

		HWND	GetHandle();

		Renderer::WindowInfo*	GetWindow();

	private:

		static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		bool	RegisterWindowClass();
	};
}
