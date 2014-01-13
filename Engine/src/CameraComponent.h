#pragma once

#include "GameComponent.h"
#include "Matrix4f.h"
#include "Shape.h"

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
	class TransformComponent;
	class WindowInfo;

	class SENTINEL_DLL CameraComponent : public GameComponent
	{
	protected:

		TransformComponent* mTransform;

	protected:

		Matrix4f			mMatrixView;
		Matrix4f			mMatrixProjection;
		Matrix4f			mMatrixFinal;

	protected:

		CameraComponent();

	public:

		const TransformComponent* GetTransform();

		virtual void		Startup();

		virtual void		Update();

		virtual void		Shutdown();

		const Matrix4f&		GetMatrixView();
		const Matrix4f&		GetMatrixProjection();
		const Matrix4f&		GetMatrixFinal();

		virtual Ray			ScreenPointToRay( UINT mouseX, UINT mouseY, UINT screenWidth = 0, UINT screenHeight = 0 ) = 0;
	};
}
