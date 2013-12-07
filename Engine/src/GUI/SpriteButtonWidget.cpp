#include "GUI/SpriteButtonWidget.h"
#include "GUI/SpriteViewWidget.h"
#include "GUI/WidgetObject.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( SpriteButtonWidget );
	DEFINE_SERIAL_CLONE( SpriteButtonWidget );

	SpriteButtonWidget::SpriteButtonWidget() :
		mFrameUp( 0 ),
		mColorUp( 1, 1, 1, 1 ),
		mFrameOver( 0 ),
		mColorOver( 0, 0.8f, 1, 1 ),
		mFrameDown( 0 ),
		mColorDown( 0, 0, 0.5f, 1 )
	{}

	SpriteButtonWidget::~SpriteButtonWidget()
	{}

	void SpriteButtonWidget::Startup()
	{
		SpriteControllerWidget::Startup();

		ViewWidget* widget = dynamic_cast< SpriteViewWidget* >( mOwner->GetView() );
		if( widget == NULL )
			throw AppException( "SpriteButtonWidget::Startup()\n" + std::string( mOwner->mName ) + " does not contain SpriteViewWidget" );
	}

	void SpriteButtonWidget::Update()
	{
		SpriteControllerWidget::Update();
		Button::Update( mIsOver );

		if( mIsActive )
		{
			SpriteViewWidget* widget = (SpriteViewWidget*)mOwner->GetView();

			switch( mState )
			{
			case UP:
				widget->mFrame = mFrameUp;
				widget->mColor = mColorUp;
				break;

			case OVER:
				widget->mFrame = mFrameOver;
				widget->mColor = mColorOver;
				break;

			case DOWN:
				widget->mFrame = mFrameDown;
				widget->mColor = mColorDown;
				break;
			}
		}
	}

	void SpriteButtonWidget::Shutdown()
	{
		SpriteControllerWidget::Shutdown();
	}

	///////////////////////////////////

	DEFINE_SERIAL_REGISTER_SAVE( SpriteButtonWidget );

	void SpriteButtonWidget::Save( Archive& archive )
	{
		Button::Save( archive );

		SpriteControllerWidget::Save( archive );

		archive.Write( &mFrameUp );
		archive.Write( &mFrameOver );
		archive.Write( &mFrameDown );
	}

	void SpriteButtonWidget::Load( Archive& archive )
	{
		Button::Load( archive );

		SpriteControllerWidget::Load( archive );

		archive.Read( &mFrameUp );
		archive.Read( &mFrameOver );
		archive.Read( &mFrameDown );
	}
}}
