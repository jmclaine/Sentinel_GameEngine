#include "Input.h"

namespace Sentinel
{
	POINT CenterHandle( HWND hWnd )
	{
		RECT screenRect;
		GetWindowRect( hWnd, &screenRect );

		POINT p;
		p.x = screenRect.left + screenRect.right / 2;
		p.y = screenRect.top + screenRect.bottom / 2;

		return p;
	}

	//////////////////////////////////////////////////////////////////////////

	HWND Mouse::mHWND = NULL;

	Mouse::Mouse()
	{
		for( UINT button = 0; button < NUM_BUTTONS; ++button )
		{
			mButtonStates[ button ]		= BUTTON_UP;
			mLastButtonStates[ button ] = BUTTON_UP;
		}
	}

	void Mouse::SetPosition( const POINT& pos )
	{
		SetCursorPos( pos.x, pos.y );
	}

	POINT Mouse::GetPosition() const
	{
		_ASSERT( mHWND );

		POINT mousePos;
		RECT rc;

		GetClientRect( mHWND, &rc );
		GetCursorPos( &mousePos );
		ScreenToClient( mHWND, &mousePos );

		mousePos.x = (LONG)((float)(mousePos.x)*1920.0f/(float)(rc.right));
		mousePos.y = (LONG)((float)(mousePos.y)*1080.0f/(float)(rc.bottom));

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

	void Mouse::Update()
	{
		for( UINT button = 0; button < NUM_BUTTONS; ++button )
		{
			mLastButtonStates[ button ] = mButtonStates[ button ];
		}
	}

	void Mouse::ProcessMessages( UINT message )
	{
		switch( message )
		{
			// Left Mouse Button.
			//
			case WM_LBUTTONDBLCLK:
				mButtonStates[ BUTTON_LEFT ]	= BUTTON_DBLCLK;
				break;

			case WM_LBUTTONDOWN:
				mButtonStates[ BUTTON_LEFT ]	= BUTTON_DOWN;
				break;

			case WM_LBUTTONUP:
				mButtonStates[ BUTTON_LEFT ]	= BUTTON_UP;
				break;

			// Right Mouse Button.
			//
			case WM_RBUTTONDBLCLK:
				mButtonStates[ BUTTON_RIGHT ]	= BUTTON_DBLCLK;
				break;

			case WM_RBUTTONDOWN:
				mButtonStates[ BUTTON_RIGHT ]	= BUTTON_DOWN;
				break;

			case WM_RBUTTONUP:
				mButtonStates[ BUTTON_RIGHT ]	= BUTTON_UP;
				break;

			// Middle Mouse Button.
			//
			case WM_MBUTTONDBLCLK:
				mButtonStates[ BUTTON_MIDDLE ] = BUTTON_DBLCLK;
				break;

			case WM_MBUTTONDOWN:
				mButtonStates[ BUTTON_MIDDLE ] = BUTTON_DOWN;
				break;

			case WM_MBUTTONUP:
				mButtonStates[ BUTTON_MIDDLE ] = BUTTON_UP;
				break;
		}
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
		{
			mLastKeyStates[ key ] = mKeyStates[ key ];
		}
	}

	void Keyboard::ProcessMessages()
	{
		for( UINT key = 0; key < NUM_KEYS; ++key )
		{
			mKeyStates[ key ] = (GetAsyncKeyState( key ) & 0x8000) == 0x8000;
		}
	}
}