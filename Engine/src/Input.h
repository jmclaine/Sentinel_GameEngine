#pragma once

#include <windows.h>

#include "Common.h"
#include "Singleton.h"

namespace Sentinel
{
	SENTINEL_DLL POINT CenterHandle( HWND hWnd = NULL );

	enum MouseButtonStatus
	{
		BUTTON_UP,
		BUTTON_DOWN,
		BUTTON_DBLCLK,
	};

	enum MouseButton
	{
		BUTTON_LEFT,
		BUTTON_RIGHT,
		BUTTON_MIDDLE,

		NUM_BUTTONS
	};

	class SENTINEL_DLL Mouse : public SingletonSafe< Mouse >
	{
		friend class SingletonSafe< Mouse >;

	private:

		int		mButtonStates[ NUM_BUTTONS ];
		int		mLastButtonStates[ NUM_BUTTONS ];

		int		mScrollDistance;

		float	mDesktopWidth;
		float	mDesktopHeight;

		///////////////////////////////////

		Mouse();

	public:

		int		Show( bool visiblity );

		void	SetPosition( const POINT& pos );
		POINT	GetPosition( HWND hWnd = NULL ) const;

		bool	IsDown( MouseButton button ) const;

		bool	DidGoUp( MouseButton button ) const;
		bool	DidGoDown( MouseButton button ) const;
		bool	DidDoubleClick( MouseButton button ) const;

		int		ScrollDistance() const;

		void	Update();

		bool	ProcessMessages( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	};

	///////////////////////////////////////////

	enum KeyStatus
	{
		KEY_UP,
		KEY_DOWN,
	};

	class SENTINEL_DLL Keyboard : public SingletonSafe< Keyboard >
	{
		friend class SingletonSafe< Keyboard >;

	private:

		static const UINT NUM_KEYS	= 256;

		int		mKeyStates[ NUM_KEYS ];
		int		mLastKeyStates[ NUM_KEYS ];

		///////////////////////////////////

		Keyboard();

	public:

		bool	IsDown( UINT key );

		bool	DidGoUp( UINT key ) const;
		bool	DidGoDown( UINT key ) const;

		void	Update();

		void	ProcessMessages();
	};
}