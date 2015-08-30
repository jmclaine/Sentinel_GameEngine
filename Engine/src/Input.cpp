#include <crtdbg.h>
#include <conio.h>

#include "Input.h"
#include "Renderer.h"

namespace Sentinel
{
	POINT CenterHandle(HWND hWnd)
	{
		RECT rect;
		GetWindowRect((!hWnd) ? GetDesktopWindow() : hWnd, &rect);

		POINT p;
		p.x = rect.left + rect.right / 2;
		p.y = rect.top + rect.bottom / 2;

		return p;
	}

	//////////////////////////////////////////////////////////////////////////

	Mouse::Mouse()
	{
		for (UINT button = 0; button < NUM_BUTTONS; ++button)
		{
			mButtonStates[button] = BUTTON_UP;
			mLastButtonStates[button] = BUTTON_UP;
		}

		mScrollDistance = 0;
	}

	Mouse& Mouse::Get()
	{
		static Mouse mouse;
		return mouse;
	}

	int Mouse::Show(bool visiblity)
	{
		return ShowCursor(visiblity);
	}

	void Mouse::SetPosition(const POINT& pos)
	{
		SetCursorPos(pos.x, pos.y);
	}

	POINT Mouse::GetPosition(HWND hWnd, bool scaleToWindow) const
	{
		POINT mousePos;

		GetCursorPos(&mousePos);

		if (hWnd)
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			ScreenToClient(hWnd, &mousePos);

			if (scaleToWindow)
			{
				mousePos.x = (LONG)((float)(mousePos.x) * (float)Renderer::WINDOW_WIDTH_BASE / (float)(rect.right));
				mousePos.y = (LONG)((float)(mousePos.y) * (float)Renderer::WINDOW_HEIGHT_BASE / (float)(rect.bottom));
			}
			else
			{
				mousePos.x = (LONG)((float)(mousePos.x));
				mousePos.y = (LONG)((float)(mousePos.y));
			}
		}

		return mousePos;
	}

	bool Mouse::IsDown(MouseButton button) const
	{
		_ASSERT(button < NUM_BUTTONS);

		return (mButtonStates[button] == BUTTON_DOWN);
	}

	bool Mouse::DidGoUp(MouseButton button) const
	{
		_ASSERT(button < NUM_BUTTONS);

		return !mButtonStates[button] && mLastButtonStates[button];
	}

	bool Mouse::DidGoDown(MouseButton button) const
	{
		_ASSERT(button < NUM_BUTTONS);

		return mButtonStates[button] && !mLastButtonStates[button];
	}

	bool Mouse::DidDoubleClick(MouseButton button) const
	{
		_ASSERT(button < NUM_BUTTONS);

		return (mButtonStates[button] == BUTTON_DBLCLK);
	}

	int Mouse::ScrollDistance() const
	{
		return mScrollDistance;
	}

	void Mouse::Update()
	{
		for (UINT button = 0; button < NUM_BUTTONS; ++button)
			mLastButtonStates[button] = mButtonStates[button];

		mScrollDistance = 0;
	}

	bool Mouse::ProcessMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			// Left Mouse Button.
			//
		case WM_LBUTTONDBLCLK:
			mButtonStates[BUTTON_LEFT] = BUTTON_DBLCLK;
			return true;

		case WM_LBUTTONDOWN:
			mButtonStates[BUTTON_LEFT] = BUTTON_DOWN;
			return true;

		case WM_LBUTTONUP:
			mButtonStates[BUTTON_LEFT] = BUTTON_UP;
			return true;

			// Right Mouse Button.
			//
		case WM_RBUTTONDBLCLK:
			mButtonStates[BUTTON_RIGHT] = BUTTON_DBLCLK;
			return true;

		case WM_RBUTTONDOWN:
			mButtonStates[BUTTON_RIGHT] = BUTTON_DOWN;
			return true;

		case WM_RBUTTONUP:
			mButtonStates[BUTTON_RIGHT] = BUTTON_UP;
			return true;

			// Middle Mouse Button.
			//
		case WM_MBUTTONDBLCLK:
			mButtonStates[BUTTON_MIDDLE] = BUTTON_DBLCLK;
			return true;

		case WM_MBUTTONDOWN:
			mButtonStates[BUTTON_MIDDLE] = BUTTON_DOWN;
			return true;

		case WM_MBUTTONUP:
			mButtonStates[BUTTON_MIDDLE] = BUTTON_UP;
			return true;

		case WM_MOUSEWHEEL:
			mScrollDistance = (int)GET_WHEEL_DELTA_WPARAM(wParam);
			return true;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	Keyboard::Keyboard()
	{}

	Keyboard& Keyboard::Get()
	{
		static Keyboard keyboard;
		return keyboard;
	}

	bool Keyboard::IsDown(UINT key)
	{
		return (mKeyStates[key] == KEY_DOWN);
	}

	bool Keyboard::DidGoUp(UINT key) const
	{
		_ASSERT(key < NUM_KEYS);

		return !mKeyStates[key] && mLastKeyStates[key];
	}

	bool Keyboard::DidGoDown(UINT key) const
	{
		_ASSERT(key < NUM_KEYS);

		return mKeyStates[key] && !mLastKeyStates[key];
	}

	void Keyboard::Update()
	{
		for (UINT key = 0; key < NUM_KEYS; ++key)
			mLastKeyStates[key] = mKeyStates[key];
	}

	void Keyboard::ProcessMessages()
	{
		for (UINT key = 0; key < NUM_KEYS; ++key)
			mKeyStates[key] = (GetAsyncKeyState(key) & 0x8000) == 0x8000;
	}
}