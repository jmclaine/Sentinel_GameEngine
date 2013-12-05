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
#include "FontSystem.h"
#include "Renderer.h"
#include "ColorRGBA.h"
#include "Vector3f.h"
#include "Matrix4f.h"
#include "Point.h"

namespace Sentinel { namespace GUI
{
	typedef std::function< void() > WidgetFunc;

	class Widget;

	class SENTINEL_DLL Root : public ListNode< Widget >
	{
	public:

		bool			mIsVisible;

		//////////////////////////////////

		Root();
		virtual ~Root();

		virtual void	Update();

		//////////////////////////////////

		virtual void	Save( Archive& archive );
		virtual void	Load( Archive& archive );
	};

	////////////////////////////////////////////////////////////////////

	class SENTINEL_DLL Widget : public Root, public Serializable
	{
	public:

		static WindowInfo*		WINDOW_INFO;
		static GameWorld*		GAME_WORLD;
		static SpriteSystem*	SPRITE_SYSTEM;
		static FontSystem*		FONT_SYSTEM;
		static Matrix4f			MATRIX_WVP;
		
	protected:

		Matrix4f		mMatrixWorld;

	public:

		Vector3f		mPosition;
		Vector3f		mRotation;
		Vector3f		mScale;

		ColorRGBA		mColor;

		bool			mPositionToWindowX;
		bool			mPositionToWindowY;

		bool			mScaleToWindowX;
		bool			mScaleToWindowY;

		Quad			mMargin;

	protected:

		bool			mIsOver;

	public:

		WidgetFunc		mActionOver;

	protected:

		Widget();

	public:

		virtual ~Widget();

		virtual void	PreUpdate();
		virtual void	Update();
		virtual void	PostUpdate();

		//////////////////////////////////

		virtual void	Save( Archive& archive );
		virtual void	Load( Archive& archive );
	};
}}
