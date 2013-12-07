#pragma once

#include "GUI/SpriteControllerWidget.h"
#include "GUI/WidgetObject.h"
#include "GUI/WidgetWorld.h"
#include "GUI/SpriteModelWidget.h"
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
		ModelWidget* widget = dynamic_cast< SpriteModelWidget* >( mOwner->GetModel() );
		if( widget == NULL )
			throw AppException( "SpriteControllerWidget::Startup()\n" + std::string( mOwner->mName ) + " does not contain SpriteTransformWidget" );
	}

	void SpriteControllerWidget::Update()
	{
		_ASSERT( mOwner->GetWorld()->mWindowInfo );

		SpriteModelWidget* model = (SpriteModelWidget*)mOwner->GetModel();

		const Vector3f& pos   = model->GetRelativePosition();
		const Vector3f& scale = model->GetRelativeScale();

		// Check for mouse being over the widget.
		//
		POINT mousePos = Mouse::Get().GetPosition( (HWND)mOwner->GetWorld()->mWindowInfo->Handle() );

		Vector3f v = model->mOrientation.Inverse().Transform( Vector3f( mousePos.x-pos.x, mousePos.y-pos.y, 0 ));

		if( v.x >= 0 && v.y >= 0 &&
			v.x <= scale.x && v.y <= scale.y )
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
	{}

	DEFINE_SERIAL_REGISTER_SAVE( SpriteControllerWidget );

	void SpriteControllerWidget::Save( Archive& archive )
	{
		ControllerWidget::Save( archive );
	}

	void SpriteControllerWidget::Load( Archive& archive )
	{
		ControllerWidget::Load( archive );
	}
}}
