#include "GUI/DrawableSprite.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( DrawableSprite );
	DEFINE_SERIAL_CLONE( DrawableSprite );

	DrawableSprite::DrawableSprite() :
		mFrame( 0 )
	{}

	DrawableSprite::~DrawableSprite()
	{}

	void DrawableSprite::Update()	
	{
		if( mIsVisible )
		{
			SPRITE_SYSTEM->Draw( mFrame, mColor, MATRIX_WVP * mMatrixWorld );
		}
	}

	///////////////////////////////////

	void DrawableSprite::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		archive.Write( &mFrame );

		Widget::Save( archive );
	}

	void DrawableSprite::Load( Archive& archive )
	{
		archive.Read( &mFrame );
		
		Widget::Load( archive );
	}
}}
