#include "GameWindow.h"
#include "Exception.h"
#include "Debug.h"
#include "Input.h"
#include "Renderer.h"
#include "WindowInfo.h"

namespace Sentinel
{
	Renderer* gRenderer = nullptr;

	GameWindow::GameWindow(UINT icon, UINT iconSmall, UINT menu, LPCSTR cursor)
	{
		mIcon = icon;
		mIconSmall = iconSmall;
		mMenu = menu;
		mCursor = cursor;

		memset(mTitle, 0, MAX_TITLE_LENGTH);
		memset(mWindowClass, 0, MAX_CLASS_LENGTH);

		mWindowInfo = nullptr;
		mRenderer = nullptr;
	}

	GameWindow::~GameWindow()
	{ }

	void GameWindow::Startup(
		Renderer* renderer, 
		HINSTANCE hInstance, 
		int nCmdShow, 
		TCHAR* title, 
		TCHAR* windowClass, 
		const WindowInfo& info)
	{
		strcpy_s(mTitle, title);
		strcpy_s(mWindowClass, windowClass);

		mINST = hInstance;

		HWND hWnd;

		if (RegisterWindowClass())
		{
			mRenderer = renderer;
			gRenderer = renderer;

			RECT rect = { 0, 0, static_cast<LONG>(info.Width()), static_cast<LONG>(info.Height()) };
			DWORD exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			DWORD dwStyle = (!info.Fullscreen()) ? WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN: WS_POPUP;
			AdjustWindowRectEx(&rect, dwStyle, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

			hWnd = CreateWindowEx(
				exStyle, mWindowClass, mTitle, dwStyle,
				0, 0, rect.right - rect.left, rect.bottom - rect.top,
				nullptr, nullptr, mINST, nullptr);

			if (!hWnd)
				throw AppException("Failed to initialize window: " + std::string(mTitle));

			//SetWindowLong(hWnd, GWL_STYLE, 0); // borderless window

			mWindowInfo = renderer->Startup(hWnd, info.Fullscreen(), info.Width(), info.Height());

			ShowWindow(hWnd, nCmdShow);
			UpdateWindow(hWnd);

			if (!mWindowInfo)
				throw AppException("Failed Renderer::Startup()");
		}
		else throw AppException("Failed GameWindow::Startup()");
	}

	void GameWindow::Update()
	{
		gRenderer = mRenderer;

		mWindowInfo->Update();
	}

	void GameWindow::Shutdown()
	{
		mRenderer->SetWindow(mWindowInfo);
		mRenderer->Shutdown();

		UnregisterClass(mWindowClass, mINST);

		SAFE_DELETE(mWindowInfo);
	}

	///////////////////////////

	const WindowInfo* GameWindow::GetInfo() const
	{
		return mWindowInfo;
	}

	///////////////////////////

	LRESULT CALLBACK GameWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Mouse::Get().ProcessMessages(hWnd, msg, wParam, lParam);

		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SIZE:
			gRenderer->ResizeBuffers(LOWORD(lParam), HIWORD(lParam));
			break;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	bool GameWindow::RegisterWindowClass()
	{
		WNDCLASSEX wcex;

		if (GetClassInfoEx(mINST, (LPCSTR)mWindowClass, &wcex))
			return true;

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = mINST;
		wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(mIcon));
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(mIconSmall));
		wcex.lpszMenuName = MAKEINTRESOURCE(mMenu);
		wcex.hCursor = LoadCursor(nullptr, mCursor);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszClassName = mWindowClass;

		if (!RegisterClassEx(&wcex))
		{
			Debug::Log(STREAM("Error: RegisterClass = " << GetLastError()));
			return false;
		}

		return true;
	}
}
