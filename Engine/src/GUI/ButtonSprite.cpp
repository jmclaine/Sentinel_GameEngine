#include "GUI/ButtonSprite.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( ButtonSprite );
	DEFINE_SERIAL_CLONE( ButtonSprite );

	ButtonSprite::ButtonSprite() :
		mFrameUp( 0 ),
		mColorUp( 1, 1, 1, 1 ),
		mFrameOver( 0 ),
		mColorOver( 0, 0.8f, 1, 1 ),
		mFrameDown( 0 ),
		mColorDown( 0, 0, 0.5f, 1 )
	{}

	ButtonSprite::~ButtonSprite()
	{}

	void ButtonSprite::Update()	
	{
		if( mIsVisible )
		{
			switch( mState )
			{
			case UP:
				SPRITE_SYSTEM->Draw( mFrameUp, mColorUp * mColor, MATRIX_WVP * mMatrixWorld );
				break;

			case OVER:
				SPRITE_SYSTEM->Draw( mFrameOver, mColorOver * mColor, MATRIX_WVP * mMatrixWorld );
				break;

			case DOWN:
				SPRITE_SYSTEM->Draw( mFrameDown, mColorDown * mColor, MATRIX_WVP * mMatrixWorld );
				break;
			}
		}
	}

	///////////////////////////////////

	void ButtonSprite::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		archive.Write( &mFrameUp );
		archive.Write( &mFrameOver );
		archive.Write( &mFrameDown );

		Button::Save( archive );
	}

	void ButtonSprite::Load( Archive& archive )
	{
		archive.Read( &mFrameUp );
		archive.Read( &mFrameOver );
		archive.Read( &mFrameDown );

		Button::Load( archive );
	}
}}
