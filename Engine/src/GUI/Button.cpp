#include "GUI/Button.h"

namespace Sentinel { namespace GUI
{
	Button::Button() :
		mActionUp( NULL ),
		mActionDown( NULL ),
		mActionClick( NULL ),
		mState( UP )
	{}

	Button::~Button()
	{}

	void Button::PreUpdate()
	{
		if( mIsVisible )
		{
			Widget::PreUpdate();

			if( mIsOver )
			{
				if( mState == UP )
					mState = OVER;
			
				if( Mouse::Get().IsDown( BUTTON_LEFT ))
				{
					mState = DOWN;

					if( mActionDown )
						mActionDown();
				}

				if( mState == DOWN && Mouse::Get().DidGoUp( BUTTON_LEFT ))
				{
					mState = UP;

					if( mActionClick )
						mActionClick();
				}
			}
			else
			if( mState != UP )
			{
				mState = UP;

				if( mActionUp )
					mActionUp();
			}
		}
	}

	void Button::PostUpdate()
	{
		if( mIsVisible )
			Widget::PostUpdate();
	}

	///////////////////////////////////

	void Button::Save( Archive& archive )
	{
		Widget::Save( archive );
	}

	void Button::Load( Archive& archive )
	{
		Widget::Load( archive );
	}
}}
