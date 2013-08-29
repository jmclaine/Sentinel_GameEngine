#pragma once

#using <System.dll>
#using <WindowsBase.dll>
#using <PresentationCore.dll>
#using <PresentationFramework.dll>

#include "Input.h"

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;

namespace Sentinel { namespace Systems
{
	public enum class MouseButton
	{
		BUTTON_LEFT,
		BUTTON_RIGHT,
		BUTTON_MIDDLE,

		NUM_BUTTONS
	};

	Point^ CenterHandle( HwndHost^ hWnd );

	public ref class WMouse
	{
	public:

		static int		Show( bool visiblity );

		static void		SetPosition( Point^ pos );
		static Point^	GetPosition( IntPtr hWnd );

		static bool		IsDown( MouseButton button );

		static bool		DidGoUp( MouseButton button );
		static bool		DidGoDown( MouseButton button );
		static bool		DidDoubleClick( MouseButton button );

		static int		ScrollDistance();

		static void		Update();

		static bool		ProcessMessages( IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam );
	};
}}
