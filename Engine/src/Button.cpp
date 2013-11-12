#include "GUI/Button.h"

namespace Sentinel { namespace GUI
{
	Button::Button() :
		mFrameUp( 0 ),
		mFrameOver( 0 ),
		mFrameDown( 0 ),
		ActionUp( NULL ),
		ActionOver( NULL ),
		ActionDown( NULL ),
		ActionClick( NULL ),
		mState( UP )
	{}

	Button::~Button()
	{}

	void Button::Update()
	{
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
	}

	void Button::Up()
	{
		mState = UP;

		if( ActionUp )
			ActionUp();
	}

	void Button::Over()
	{
		mState = OVER;

		if( ActionOver )
			ActionOver();
	}

	void Button::Down()
	{
		mState = DOWN;

		if( ActionDown )
			ActionDown();
	}
	
	void Button::Click()
	{
		Up();

		if( ActionClick )
			ActionClick();
	}
}}
