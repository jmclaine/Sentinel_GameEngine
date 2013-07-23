#pragma once

#include <windows.h>

#include "Singleton.h"

namespace Sentinel
{
	POINT CenterHandle( HWND hWnd = NULL );

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

	class Mouse : public Singleton< Mouse >
	{
	private:

		int mButtonStates[ NUM_BUTTONS ];
		int mLastButtonStates[ NUM_BUTTONS ];

	public:

		static HWND mHWND;

		/////////////////////////////////

		Mouse();

		void  SetPosition( const POINT& pos );
		POINT GetPosition() const;

		bool IsDown( MouseButton button ) const;

		bool DidGoUp( MouseButton button ) const;
		bool DidGoDown( MouseButton button ) const;
		bool DidDoubleClick( MouseButton button ) const;

		void Update();

		void ProcessMessages( UINT message );
	};

	///////////////////////////////////////////

	enum KeyStatus
	{
		KEY_UP,
		KEY_DOWN,
	};

	class Keyboard : public Singleton< Keyboard >
	{
	private:

		static const UINT NUM_KEYS	= 256;

		int mKeyStates[ NUM_KEYS ];
		int mLastKeyStates[ NUM_KEYS ];

	public:

		Keyboard();
		
		bool IsDown( UINT key );

		bool DidGoUp( UINT key ) const;
		bool DidGoDown( UINT key ) const;

		void Update();

		void ProcessMessages();
	};
}