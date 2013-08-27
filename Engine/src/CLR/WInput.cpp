#include "WInput.h"

namespace Sentinel { namespace Systems
{
	Point^ CenterHandle( HwndHost^ hWnd )
	{
		POINT p = Sentinel::CenterHandle( (HWND)hWnd->Handle.ToPointer() );
		return gcnew Point( p.x, p.y );
	}

	/////////////////////////////////////////////////////

	int WMouse::Show( bool visiblity )
	{
		return Mouse::Inst()->Show( visiblity );
	}

	void WMouse::SetPosition( Point^ pos )
	{
		POINT p;
		p.x = (LONG)pos->X;
		p.y = (LONG)pos->Y;
		Mouse::Inst()->SetPosition( p );
	}

	Point^ WMouse::GetPosition( IntPtr hWnd )
	{
		POINT p = Mouse::Inst()->GetPosition( (HWND)hWnd.ToPointer() );
		
		return gcnew Point( p.x, p.y );
	}

	bool WMouse::IsDown( MouseButton button )
	{
		return Mouse::Inst()->IsDown( (Sentinel::MouseButton)button );
	}

	bool WMouse::DidGoUp( MouseButton button )
	{
		return Mouse::Inst()->DidGoUp( (Sentinel::MouseButton)button );
	}

	bool WMouse::DidGoDown( MouseButton button )
	{
		return Mouse::Inst()->DidGoDown( (Sentinel::MouseButton)button );
	}

	bool WMouse::DidDoubleClick( MouseButton button )
	{
		return Mouse::Inst()->DidDoubleClick( (Sentinel::MouseButton)button );
	}

	int WMouse::ScrollDistance()
	{
		return Mouse::Inst()->ScrollDistance();
	}

	void WMouse::Update()
	{
		Mouse::Inst()->Update();
	}

	bool WMouse::ProcessMessages( IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam )
	{
		return Mouse::Inst()->ProcessMessages( (HWND)hWnd.ToPointer(), msg, (WPARAM)wParam.ToPointer(), (LPARAM)lParam.ToPointer() );
	}
}}
