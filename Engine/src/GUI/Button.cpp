#include "GUI/Button.h"
#include "Input.h"

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

	void Button::Update( bool isOver )
	{
		if( isOver )
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

	///////////////////////////////////

	void Button::Save( Archive& archive )
	{
		//SerialFunctionFactory::Get().Find( mActionUp );
	}

	void Button::Load( Archive& archive )
	{}
}}
