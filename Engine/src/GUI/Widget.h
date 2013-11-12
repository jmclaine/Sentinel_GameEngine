#pragma once
/*
All Widgets have a static Sprite skin that contains 
the images to use for each type.
*/
#include <vector>

#include "Common.h"
#include "ListNode.h"
#include "Input.h"
#include "Sprite.h"
#include "Renderer.h"
#include "Point.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL Widget : public ListNode< Widget >
	{
	public:

		static Renderer*					RENDERER;
		static std::shared_ptr< Sprite >	SKIN;

		static void DoNothing();

	protected:

		Quad			mDimension;

	public:

		Widget();
		virtual ~Widget();

		virtual void	Update();

		// Check if the mouse cursor is over this widget.
		//
		bool			IsOver();

		// Set position relative to parent.
		//
		void			SetPosition( const Point2i& pos );
		Point2i			GetPosition();
	};
}}
