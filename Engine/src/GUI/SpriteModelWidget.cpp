#include "GUI/SpriteModelWidget.h"
#include "GUI/WidgetObject.h"
#include "GUI/WidgetWorld.h"
#include "Renderer.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( SpriteModelWidget );
	DEFINE_SERIAL_CLONE( SpriteModelWidget );

	SpriteModelWidget::SpriteModelWidget() :
		mDidUpdatePosition( false ),
		mDidUpdateScale( false ),
		mPositionToWindowX( false ),
		mPositionToWindowY( false ),
		mScaleToWindowX( false ),
		mScaleToWindowY( false )
	{}

	SpriteModelWidget::~SpriteModelWidget()
	{}

	void SpriteModelWidget::Startup()
	{
		_ASSERT( mOwner->GetWorld()->mWindowInfo );
	}

	void SpriteModelWidget::Update()
	{
		// Position widget.
		//
		UpdateRelativePosition();
		UpdateRelativeScale();

		mMatrixWorld.World( mRelativePosition, mOrientation, mRelativeScale );

		mDidUpdatePosition = false;
		mDidUpdateScale    = false;
	}

	void SpriteModelWidget::Shutdown()
	{}

	void SpriteModelWidget::UpdateRelativePosition()
	{
		_ASSERT( mOwner->GetWorld()->mWindowInfo );

		if( !mDidUpdatePosition )
		{
			mRelativePosition = mPosition;

			if( mPositionToWindowX )
				mRelativePosition.x *= mOwner->GetWorld()->mWindowInfo->WidthRatio();
		
			if( mPositionToWindowY )
				mRelativePosition.y *= mOwner->GetWorld()->mWindowInfo->HeightRatio();

			mRelativePosition.x += mMargin.left;
			mRelativePosition.y += mMargin.top;

			mDidUpdatePosition = true;
		}
	}

	const Vector3f& SpriteModelWidget::GetRelativePosition()
	{
		UpdateRelativePosition();

		return mRelativePosition;
	}

	void SpriteModelWidget::UpdateRelativeScale()
	{
		_ASSERT( mOwner->GetWorld()->mWindowInfo );

		if( !mDidUpdateScale )
		{
			mRelativeScale = mScale;

			if( mScaleToWindowX )
				mRelativeScale.x *= mOwner->GetWorld()->mWindowInfo->WidthRatio();
		
			if( mScaleToWindowY )
				mRelativeScale.y *= mOwner->GetWorld()->mWindowInfo->HeightRatio();

			mRelativeScale.x += mMargin.right;
			mRelativeScale.y += mMargin.bottom;

			mDidUpdateScale = true;
		}
	}

	const Vector3f& SpriteModelWidget::GetRelativeScale()
	{
		UpdateRelativeScale();

		return mRelativeScale;
	}

	DEFINE_SERIAL_REGISTER_SAVE( SpriteModelWidget );

	void SpriteModelWidget::Save( Archive& archive )
	{
		ModelWidget::Save( archive );

		archive.Write( &mPositionToWindowX );
		archive.Write( &mPositionToWindowY );

		archive.Write( &mScaleToWindowX );
		archive.Write( &mScaleToWindowY );
	}

	void SpriteModelWidget::Load( Archive& archive )
	{
		ModelWidget::Load( archive );

		archive.Read( &mPositionToWindowX );
		archive.Read( &mPositionToWindowY );

		archive.Read( &mScaleToWindowX );
		archive.Read( &mScaleToWindowY );
	}
}}
