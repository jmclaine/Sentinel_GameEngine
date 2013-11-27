#include "GUI/Button.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( Button );
	DEFINE_SERIAL_CLONE( Button );

	Button::Button() :
		mFrameUp( 0 ),
		mFrameOver( 1 ),
		mFrameDown( 2 ),
		mActionUp( NULL ),
		mActionDown( NULL ),
		mActionClick( NULL ),
		mState( UP )
	{}

	Button::~Button()
	{}

	void Button::Update()
	{
		Widget::PreUpdate();

		// Check state of button.
		//
		if( IsOver() )
		{
			if( mState == UP )
				Over();
			
			if( Mouse::Get().IsDown( BUTTON_LEFT ))
				Down();

			if( mState == DOWN && Mouse::Get().DidGoUp( BUTTON_LEFT ))
				Click();
		}
		else
		if( mState != UP )
		{
			Up();
		}

		// Draw button.
		//
		switch( mState )
		{
		case UP:
			GAME_WORLD->mSpriteSystem->Draw( mFrameUp, mColor, MATRIX_WVP * mMatrixWorld );
			break;

		case OVER:
			GAME_WORLD->mSpriteSystem->Draw( mFrameOver, mColor, MATRIX_WVP * mMatrixWorld );
			break;

		case DOWN:
			GAME_WORLD->mSpriteSystem->Draw( mFrameDown, mColor, MATRIX_WVP * mMatrixWorld );
			break;
		}

		// Update children.
		//
		Widget::PostUpdate();
	}

	void Button::Up()
	{
		mState = UP;

		if( mActionUp )
			mActionUp();
	}

	void Button::Over()
	{
		mState = OVER;

		Widget::Over();
	}

	void Button::Down()
	{
		mState = DOWN;

		if( mActionDown )
			mActionDown();
	}
	
	void Button::Click()
	{
		Up();

		if( mActionClick )
			mActionClick();
	}

	///////////////////////////////////

	void Button::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		archive.Write( &mFrameUp );
		archive.Write( &mFrameOver );
		archive.Write( &mFrameDown );

		Widget::Save( archive );
	}

	void Button::Load( Archive& archive )
	{
		archive.Read( &mFrameUp );
		archive.Read( &mFrameOver );
		archive.Read( &mFrameDown );

		Widget::Load( archive );
	}
}}
