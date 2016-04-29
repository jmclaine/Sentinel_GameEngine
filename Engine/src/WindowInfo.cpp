#include "WindowInfo.h"

#include <Windows.h>

namespace Sentinel
{
	UINT WindowInfo::BASE_WIDTH = 1920;
	UINT WindowInfo::BASE_HEIGHT = 1080;

	WindowInfo::WindowInfo()
	{
		mFullscreen = false;

		mWidth = 1920;
		mHeight = 1080;

		mWidthRatio = 1.0f;
		mHeightRatio = 1.0f;
	}

	WindowInfo::~WindowInfo()
	{ }

	bool WindowInfo::Fullscreen() const
	{
		return mFullscreen;
	}

	UINT WindowInfo::Width() const
	{
		return mWidth;
	}

	UINT WindowInfo::Height() const
	{
		return mHeight;
	}

	float WindowInfo::WidthRatio() const
	{
		return mWidthRatio;
	}

	float WindowInfo::HeightRatio() const
	{
		return mHeightRatio;
	}

	void* WindowInfo::Handle() const
	{
		return mHandle;
	}

	void WindowInfo::Update()
	{
		static RECT rect;
		GetClientRect((HWND)mHandle, &rect);

		mWidth = (UINT)(rect.right - rect.left);
		mHeight = (UINT)(rect.bottom - rect.top);

		mWidthRatio = (float)mWidth / (float)BASE_WIDTH;
		mHeightRatio = (float)mHeight / (float)BASE_HEIGHT;
	}
}
