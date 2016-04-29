#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	class SENTINEL_DLL WindowInfo
	{
		friend class Renderer;

	protected:
		void* mHandle;

		bool mFullscreen;

		UINT mWidth;
		UINT mHeight;

		float mWidthRatio;
		float mHeightRatio;

	public:
		static UINT BASE_WIDTH;
		static UINT BASE_HEIGHT;

	public:
		WindowInfo();
		~WindowInfo();

		bool Fullscreen() const;

		UINT Width() const;
		UINT Height() const;

		float WidthRatio() const;
		float HeightRatio() const;

		void* Handle() const;

		void Update();
	};
}
