#pragma once

#include "GUI\ViewWidget.h"

namespace Sentinel { namespace GUI
{
	ViewWidget::ViewWidget() :
		mIsVisible( true )
	{
		mType = Widget::VIEW;
	}

	ViewWidget::~ViewWidget()
	{}

	void ViewWidget::Save( Archive& archive )
	{
		archive.Write( &mIsVisible );
	}

	void ViewWidget::Load( Archive& archive )
	{
		archive.Read( &mIsVisible );
	}
}}
