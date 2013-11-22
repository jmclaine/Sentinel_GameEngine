#pragma once
/*
All Widgets use the SpriteSystem to render images by frame.
*/
#include <vector>
#include <functional>
#include <memory>

#include "Common.h"
#include "ListNode.h"
#include "Serializable.h"
#include "Archive.h"
#include "Input.h"
#include "GameWorld.h"
#include "SpriteSystem.h"
#include "ColorRGBA.h"
#include "Vector3f.h"
#include "Matrix4f.h"

namespace Sentinel { namespace GUI
{
#define WIDGET_BIND( func ) std::bind( [=]() { func(); } );

	typedef std::function< void() > WidgetFunc;

	class SENTINEL_DLL Widget : public ListNode< Widget >, public Serializable
	{
	public:

		static HWND			WINDOW;
		static GameWorld*	GAME_WORLD;
		static Matrix4f		MATRIX_WVP;
		
	protected:

		Matrix4f		mMatrixWorld;

	public:

		Vector3f		mPosition;
		Vector3f		mRotation;
		Vector3f		mScale;

		ColorRGBA		mColor;

		bool			mIsVisible;
		bool			mPositionToWindow;
		bool			mScaleToWindow;

		WidgetFunc		mActionOver;

		////////////////////////////////

		Widget();
		virtual ~Widget();

	protected:

		virtual void	PreUpdate();

	public:

		virtual void	Update();

	protected:

		virtual void	PostUpdate();

	public:

		bool			IsOver();

	protected:

		virtual void	Over();

	public:

		virtual void	Save( Archive& archive );

		virtual void	Load( Archive& archive );
	};
}}
