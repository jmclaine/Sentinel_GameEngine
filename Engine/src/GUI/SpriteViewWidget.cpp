#include "GUI/SpriteViewWidget.h"
#include "GUI/WidgetObject.h"
#include "GUI/WidgetWorld.h"
#include "GUI/ModelWidget.h"
#include "SpriteSystem.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( SpriteViewWidget );
	DEFINE_SERIAL_CLONE( SpriteViewWidget );

	SpriteViewWidget::SpriteViewWidget() :
		mFrame( 0 ),
		mColor( 1, 1, 1, 1 )
	{}

	SpriteViewWidget::~SpriteViewWidget()
	{}

	////////////////////////////////////////////////////

	void SpriteViewWidget::Startup()
	{}

	void SpriteViewWidget::Update()
	{
		_ASSERT( mOwner->GetModel() );
		_ASSERT( mOwner->GetWorld()->mSpriteSystem );

		if( mIsVisible )
			mOwner->GetWorld()->mSpriteSystem->Draw( mFrame, mColor, mOwner->GetWorld()->mMatrixWVP * mOwner->GetModel()->GetMatrixWorld() );
	}

	void SpriteViewWidget::Shutdown()
	{}

	////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER_SAVE( SpriteViewWidget );

	void SpriteViewWidget::Save( Archive& archive )
	{
		ViewWidget::Save( archive );

		archive.Write( &mFrame );
		archive.Write( mColor.Ptr(), ar_sizeof( mColor ));
	}

	void SpriteViewWidget::Load( Archive& archive )
	{
		ViewWidget::Load( archive );

		archive.Read( &mFrame );
		archive.Read( mColor.Ptr(), ar_sizeof( mColor ));
	}
}}
