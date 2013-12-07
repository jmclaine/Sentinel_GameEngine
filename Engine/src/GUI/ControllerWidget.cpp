#pragma once

#include "GUI\ControllerWidget.h"

namespace Sentinel { namespace GUI
{
	ControllerWidget::ControllerWidget() :
		mIsActive( true ),
		mIsOver( false ),
		mActionOver( NULL )
	{
		mType = Widget::CONTROLLER;
	}

	ControllerWidget::~ControllerWidget()
	{}

	void ControllerWidget::Save( Archive& archive )
	{
		archive.Write( &mIsActive );
	}

	void ControllerWidget::Load( Archive& archive )
	{
		archive.Read( &mIsActive );
	}
}}
