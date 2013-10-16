#include "WInput.h"

namespace Sentinel { namespace Wrapped
{
	Point^ CenterHandle( System::IntPtr hWnd )
	{
		POINT p = Sentinel::CenterHandle( (HWND)hWnd.ToPointer() );
		return gcnew Point( p.x, p.y );
	}

	/////////////////////////////////////////////////////

	int WMouse::Show( bool visiblity )
	{
		return Mouse::Get().Show( visiblity );
	}

	void WMouse::SetPosition( Point^ pos )
	{
		POINT p;
		p.x = (LONG)pos->X;
		p.y = (LONG)pos->Y;
		Mouse::Get().SetPosition( p );
	}

	Point^ WMouse::GetPosition( System::IntPtr hWnd )
	{
		POINT p = Mouse::Get().GetPosition( (HWND)hWnd.ToPointer() );
		
		return gcnew Point( p.x, p.y );
	}

	bool WMouse::IsDown( MouseButton button )
	{
		return Mouse::Get().IsDown( (Sentinel::MouseButton)button );
	}

	bool WMouse::DidGoUp( MouseButton button )
	{
		return Mouse::Get().DidGoUp( (Sentinel::MouseButton)button );
	}

	bool WMouse::DidGoDown( MouseButton button )
	{
		return Mouse::Get().DidGoDown( (Sentinel::MouseButton)button );
	}

	bool WMouse::DidDoubleClick( MouseButton button )
	{
		return Mouse::Get().DidDoubleClick( (Sentinel::MouseButton)button );
	}

	int WMouse::ScrollDistance()
	{
		return Mouse::Get().ScrollDistance();
	}

	void WMouse::Update()
	{
		Mouse::Get().Update();
	}

	bool WMouse::ProcessMessages( System::IntPtr hWnd, int msg, System::IntPtr wParam, System::IntPtr lParam )
	{
		return Mouse::Get().ProcessMessages( (HWND)hWnd.ToPointer(), msg, (WPARAM)wParam.ToPointer(), (LPARAM)lParam.ToPointer() );
	}
}}
