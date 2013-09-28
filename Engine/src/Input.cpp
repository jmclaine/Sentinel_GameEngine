#include <crtdbg.h>

#include "Input.h"

namespace Sentinel
{
	POINT CenterHandle( HWND hWnd )
	{
		RECT screenRect;
		GetWindowRect( (!hWnd) ? GetDesktopWindow() : hWnd, &screenRect );

		POINT p;
		p.x = screenRect.left + screenRect.right / 2;
		p.y = screenRect.top + screenRect.bottom / 2;

		return p;
	}

	//////////////////////////////////////////////////////////////////////////

	Mouse::Mouse()
	{
		for( UINT button = 0; button < NUM_BUTTONS; ++button )
		{
			mButtonStates[ button ]		= BUTTON_UP;
			mLastButtonStates[ button ] = BUTTON_UP;
		}

		mScrollDistance = 0;

		RECT rc;
		GetWindowRect( GetDesktopWindow(), &rc );

		mDesktopWidth  = (float)(rc.left - rc.right);
		mDesktopHeight = (float)(rc.top  - rc.bottom);
	}

	int Mouse::Show( bool visiblity )
	{
		return ShowCursor( visiblity );
	}

	void Mouse::SetPosition( const POINT& pos )
	{
		SetCursorPos( pos.x, pos.y );
	}

	POINT Mouse::GetPosition( HWND hWnd ) const
	{
		POINT mousePos;
		
		GetCursorPos( &mousePos );

		if( hWnd )
		{
			RECT rc;
			GetClientRect( hWnd, &rc );
			ScreenToClient( hWnd, &mousePos );

			mousePos.x = (LONG)((float)(mousePos.x) * mDesktopWidth / (float)(rc.right));
			mousePos.y = (LONG)((float)(mousePos.y) * mDesktopHeight / (float)(rc.bottom));
		}
		
		return mousePos;
	}

	bool Mouse::IsDown( MouseButton button ) const
	{
		_ASSERT( button < NUM_BUTTONS );

		return (mButtonStates[ button ] == BUTTON_DOWN);
	}

	bool Mouse::DidGoUp( MouseButton button ) const
	{
		_ASSERT( button < NUM_BUTTONS );

		return !mButtonStates[ button ] && mLastButtonStates[ button ];
	}

	bool Mouse::DidGoDown( MouseButton button ) const
	{
		_ASSERT( button < NUM_BUTTONS );

		return mButtonStates[ button ] && !mLastButtonStates[ button ];
	}

	bool Mouse::DidDoubleClick( MouseButton button ) const
	{
		_ASSERT( button < NUM_BUTTONS );

		return (mButtonStates[ button ] == BUTTON_DBLCLK);
	}

	int Mouse::ScrollDistance() const
	{
		return mScrollDistance;
	}

	void Mouse::Update()
	{
		for( UINT button = 0; button < NUM_BUTTONS; ++button )
			mLastButtonStates[ button ] = mButtonStates[ button ];
	
		mScrollDistance = 0;
	}

	bool Mouse::ProcessMessages( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		switch( msg )
		{
			// Left Mouse Button.
			//
			case WM_LBUTTONDBLCLK:
				mButtonStates[ BUTTON_LEFT ]	= BUTTON_DBLCLK;
				return true;

			case WM_LBUTTONDOWN:
				mButtonStates[ BUTTON_LEFT ]	= BUTTON_DOWN;
				return true;

			case WM_LBUTTONUP:
				mButtonStates[ BUTTON_LEFT ]	= BUTTON_UP;
				return true;

			// Right Mouse Button.
			//
			case WM_RBUTTONDBLCLK:
				mButtonStates[ BUTTON_RIGHT ]	= BUTTON_DBLCLK;
				return true;

			case WM_RBUTTONDOWN:
				mButtonStates[ BUTTON_RIGHT ]	= BUTTON_DOWN;
				return true;

			case WM_RBUTTONUP:
				mButtonStates[ BUTTON_RIGHT ]	= BUTTON_UP;
				return true;

			// Middle Mouse Button.
			//
			case WM_MBUTTONDBLCLK:
				mButtonStates[ BUTTON_MIDDLE ]	= BUTTON_DBLCLK;
				return true;

			case WM_MBUTTONDOWN:
				mButtonStates[ BUTTON_MIDDLE ]	= BUTTON_DOWN;
				return true;

			case WM_MBUTTONUP:
				mButtonStates[ BUTTON_MIDDLE ]	= BUTTON_UP;
				return true;

			case WM_MOUSEWHEEL:
				mScrollDistance = (int)GET_WHEEL_DELTA_WPARAM( wParam );
				return true;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	Keyboard::Keyboard()
	{}

	bool Keyboard::IsDown( UINT key )
	{
		bool keyDown = (GetAsyncKeyState( key ) & 0x8000 ) == 0x8000;

		(keyDown) ? mKeyStates[ key ] = KEY_DOWN : mKeyStates[ key ] = KEY_UP;

		return keyDown;
	}

	bool Keyboard::DidGoUp( UINT key ) const
	{
		_ASSERT( key < NUM_KEYS );

		return !mKeyStates[ key ] && mLastKeyStates[ key ];
	}

	bool Keyboard::DidGoDown( UINT key ) const
	{
		_ASSERT( key < NUM_KEYS );

		return mKeyStates[ key ] && !mLastKeyStates[ key ];
	}

	void Keyboard::Update()
	{
		for( UINT key = 0; key < NUM_KEYS; ++key )
			mLastKeyStates[ key ] = mKeyStates[ key ];
	}

	void Keyboard::ProcessMessages()
	{
		for( UINT key = 0; key < NUM_KEYS; ++key )
			mKeyStates[ key ] = (GetAsyncKeyState( key ) & 0x8000) == 0x8000;
	}
}