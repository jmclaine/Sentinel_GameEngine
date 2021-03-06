#pragma once

#include <Windows.h>

#include "Sentinel.h"

namespace Sentinel
{
	#define MAX_TITLE_LENGTH	16
	#define MAX_CLASS_LENGTH	24

	class Renderer;
	class WindowInfo;

	// Helper class for quick window creation setup
	// specifically for the Renderer.
	//
	class SENTINEL_DLL GameWindow
	{
	private:

		HINSTANCE mINST;

		UINT mIcon;
		UINT mIconSmall;
		UINT mMenu;
		LPCSTR mCursor;

		char mTitle[MAX_TITLE_LENGTH];
		char mWindowClass[MAX_CLASS_LENGTH];

		WindowInfo* mWindowInfo;
		Renderer* mRenderer;

	public:

		GameWindow(UINT icon, UINT iconSmall, UINT menu, LPCSTR cursor = IDC_ARROW);
		~GameWindow();

		void Startup(
			Renderer* renderer, 
			HINSTANCE hInstance, 
			int nCmdShow, 
			char* title, 
			char* windowClass, 
			const WindowInfo& info);

		void Update();

		void Shutdown();

		///////////////////////////

		const WindowInfo* GetInfo() const;

	private:

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool RegisterWindowClass();
	};
}
