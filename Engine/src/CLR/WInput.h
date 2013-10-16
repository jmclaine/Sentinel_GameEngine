#pragma once

#using <System.dll>
#using <WindowsBase.dll>

#include "Input.h"

using namespace System::Windows;

namespace Sentinel { namespace Wrapped
{
	public enum class MouseButton
	{
		BUTTON_LEFT,
		BUTTON_RIGHT,
		BUTTON_MIDDLE,

		NUM_BUTTONS
	};

	Point^ CenterHandle( System::IntPtr hWnd );

	public ref class WMouse
	{
	public:

		static int		Show( bool visiblity );

		static void		SetPosition( Point^ pos );
		static Point^	GetPosition( System::IntPtr hWnd );

		static bool		IsDown( MouseButton button );

		static bool		DidGoUp( MouseButton button );
		static bool		DidGoDown( MouseButton button );
		static bool		DidDoubleClick( MouseButton button );

		static int		ScrollDistance();

		static void		Update();

		static bool		ProcessMessages( System::IntPtr hWnd, int msg, System::IntPtr wParam, System::IntPtr lParam );
	};
}}
