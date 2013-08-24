#pragma once
/*
Created a new GameWindow specific for the CLR
due to the major differences in instantiation
of the windows between C# / WPF and C++
*/
#include "WRenderer.h"
#include "GameWindow.h"

namespace Sentinel { namespace Systems
{
	public ref class WGameWindow : public HwndHost
	{
	private:

		HWND				mHWND;
		HINSTANCE			mINST;

		wchar_t* 			mTitle;
		wchar_t* 			mWindowClass;
		
		WWindowInfo^		mWindowInfo;

	public:

		WGameWindow();
		~WGameWindow();

		void				Startup( String^ title, String^ windowClass, WWindowInfo^ info );
		
		void				Shutdown();

		///////////////////////////

		void				SetActive();

		bool				ShareResources( WGameWindow^ window );

		WWindowInfo^		GetInfo();

	protected:
		
		virtual IntPtr		WndProc( IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled ) override;

		virtual void		OnRenderSizeChanged( SizeChangedInfo^ sizeInfo ) override;

		virtual void		OnRender( DrawingContext^ drawingContext ) override;

		virtual void		OnMouseDown( MouseButtonEventArgs^ e ) override;

		virtual void		DestroyWindowCore( HandleRef hwnd ) override;

		bool				RegisterWindowClass();

		virtual HandleRef	BuildWindowCore( HandleRef hwndParent ) override;
	};
}}
