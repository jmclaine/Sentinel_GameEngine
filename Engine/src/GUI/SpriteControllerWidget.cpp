#pragma once

#include "GUI/SpriteControllerWidget.h"
#include "GUI/WidgetObject.h"
#include "GUI/WidgetWorld.h"
#include "GUI/ModelWidget.h"
#include "Matrix4f.h"
#include "Renderer.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( SpriteControllerWidget );
	DEFINE_SERIAL_CLONE( SpriteControllerWidget );

	SpriteControllerWidget::SpriteControllerWidget()
	{}

	SpriteControllerWidget::~SpriteControllerWidget()
	{}

	///////////////////////////////////

	void SpriteControllerWidget::Startup()
	{
		ModelWidget* model = mOwner->GetModel();

		if( model == NULL )
			throw AppException( "SpriteControllerWidget::Startup()\n" + std::string( mOwner->mName ) + " does not contain ModelWidget" );

		mBasePosition = model->mPosition;
		mBaseScale    = model->mScale;
	}

	void SpriteControllerWidget::Update()
	{
		_ASSERT( mOwner->GetWorld()->mWindowInfo );

		ModelWidget* model = mOwner->GetModel();

		// Adjust position.
		//
		Vector3f* pos = &model->mPosition;
		
		*pos = mBasePosition;

		if( mPositionToWindowX )
			pos->x *= mOwner->GetWorld()->mWindowInfo->WidthRatio();
		
		if( mPositionToWindowY )
			pos->y *= mOwner->GetWorld()->mWindowInfo->HeightRatio();

		pos->x += mMargin.left;
		pos->y += mMargin.top;

		// Adjust scale.
		//
		Vector3f* scale = &model->mScale;

		*scale = mBaseScale;

		if( mScaleToWindowX )
			scale->x *= mOwner->GetWorld()->mWindowInfo->WidthRatio();
		
		if( mScaleToWindowY )
			scale->y *= mOwner->GetWorld()->mWindowInfo->HeightRatio();

		scale->x += mMargin.right;
		scale->y += mMargin.bottom;

		// Check for mouse being over the widget.
		//
		POINT mousePos = Mouse::Get().GetPosition( (HWND)mOwner->GetWorld()->mWindowInfo->Handle() );

		Vector3f v = model->mOrientation.Inverse().Transform( Vector3f( mousePos.x-pos->x, mousePos.y-pos->y, 0 ));

		if( v.x >= 0 && v.y >= 0 &&
			v.x <= scale->x && v.y <= scale->y )
		{
			if( !mIsOver )
			{
				mIsOver = true;
				
				if( mActionOver )
					mActionOver();
			}
		}
		else
		{
			mIsOver = false;
		}
	}

	void SpriteControllerWidget::Shutdown()
	{
		ModelWidget* model = mOwner->GetModel();

		model->mPosition = mBasePosition;
		model->mScale    = mBaseScale;
	}

	DEFINE_SERIAL_REGISTER_SAVE( SpriteControllerWidget );

	void SpriteControllerWidget::Save( Archive& archive )
	{
		ControllerWidget::Save( archive );

		archive.Write( &mPositionToWindowX );
		archive.Write( &mPositionToWindowY );

		archive.Write( &mScaleToWindowX );
		archive.Write( &mScaleToWindowY );

		archive.Write( mMargin.Ptr(), ar_sizeof( mMargin ));
	}

	void SpriteControllerWidget::Load( Archive& archive )
	{
		ControllerWidget::Load( archive );

		archive.Read( &mPositionToWindowX );
		archive.Read( &mPositionToWindowY );

		archive.Read( &mScaleToWindowX );
		archive.Read( &mScaleToWindowY );

		archive.Read( mMargin.Ptr(), ar_sizeof( mMargin ));
	}
}}
