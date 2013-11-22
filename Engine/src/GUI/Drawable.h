#pragma once

#include "GUI/Widget.h"

namespace Sentinel 
{ 
	class Mesh;

namespace GUI
{
	class SENTINEL_DLL Drawable : public Widget
	{
		DECLARE_SERIAL();

	public:

		std::shared_ptr< Mesh > mMesh;

	protected:

		bool			mIsOver;

	public:

		WidgetFunc		mActionOver;
		
		//////////////////////////////////

		Drawable();
		virtual ~Drawable();

		virtual void	Update();

	protected:

		virtual void	Over();
	};
}}
