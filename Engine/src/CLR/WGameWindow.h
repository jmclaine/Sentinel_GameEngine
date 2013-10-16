#pragma once
/*
Created a new GameWindow specific for the CLR
due to the major differences in instantiation
of the windows between C# / WPF and C++

WGameWindow should be inherited to use input.

public class GameWindow : WGameWindow
{}
*/
#using <System.dll>
#using <WindowsBase.dll>
#using <PresentationCore.dll>
#using <PresentationFramework.dll>

#include <Windows.h>

#include "GameWindow.h"
#include "CDPI.h"

using namespace System::Windows;
using namespace System::Windows::Interop;
using namespace System::Windows::Media;
using namespace System::Windows::Input;
using namespace System::Runtime::InteropServices;

namespace Sentinel { namespace Wrapped
{
	ref class WRenderer;
	ref class WWindowInfo;

	public ref class WGameWindow : public HwndHost
	{
	private:

		HWND				mHWND;
		HINSTANCE			mINST;

		wchar_t* 			mTitle;
		wchar_t* 			mWindowClass;
		
		WWindowInfo^		mWindowInfo;

		WRenderer^			mRenderer;

		CDPI*				mCDPI;	// fixes aspect ratio within window

	public:

		WGameWindow();
		~WGameWindow();
		!WGameWindow();

		virtual void		Release();

		/////////////////////////////////

		virtual void		Startup( WRenderer^ renderer, System::String^ title, System::String^ windowClass, WWindowInfo^ info );
		
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
		
		virtual System::IntPtr WndProc( System::IntPtr hwnd, int msg, System::IntPtr wParam, System::IntPtr lParam, bool% handled ) override;

		virtual void		OnRenderSizeChanged( SizeChangedInfo^ sizeInfo ) override;

		virtual void		OnRender( DrawingContext^ drawingContext ) override;

		virtual void		OnMouseDown( MouseButtonEventArgs^ e ) override;

		virtual void		DestroyWindowCore( HandleRef hwnd ) override;

		bool				RegisterWindowClass();

		virtual HandleRef	BuildWindowCore( HandleRef hwndParent ) override;
	};
}}
