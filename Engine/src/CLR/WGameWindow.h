#pragma once
/*
Created a new GameWindow specific for the CLR
due to the major differences in instantiation
of the windows between C# / WPF and C++

WGameWindow should be inherited to use input.

public class GameWindow : WGameWindow
{}
*/
#include "WRenderer.h"

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

		virtual void		Startup( String^ title, String^ windowClass, WWindowInfo^ info );
		
		virtual void		Update();

		virtual void		Shutdown();

	protected:

		virtual void		OnLeftMouseDown();
		virtual void		OnRightMouseDown();
		virtual void		OnMiddleMouseDown();

		virtual void		OnLeftMouseUp();
		virtual void		OnRightMouseUp();
		virtual void		OnMiddleMouseUp();

		virtual void		OnLeftMouseDoubleClick();
		virtual void		OnRightMouseDoubleClick();
		virtual void		OnMiddleMouseDoubleClick();

		virtual void		OnMouseScroll();
		virtual void		OnMouseMove();

	public:

		void				SetActive();

		bool				ShareResources( WGameWindow^ window );

		WWindowInfo^		GetInfo();

		System::IntPtr^		GetHWND();

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
