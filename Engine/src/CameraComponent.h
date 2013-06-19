#pragma once

#include "MathLib.h"
#include "TransformComponent.h"

#ifndef WIN32
    #define HWND void*
    #define GetClientRect( x, y );

    struct RECT
    {
        int right;
        int top;
        int left;
        int bottom;
    };
#endif

namespace Sentinel
{
	class CameraComponent : public GameComponent
	{
	public:

		TransformComponent* mTransform;

		HWND		mHWND;
		
		mat4f		mMatrixView;
		mat4f		mMatrixProjection;
		mat4f		mMatrixFinal;

	protected:

		CameraComponent();

	public:

		void Startup();

		void Update();

		void Shutdown();
	};
}
