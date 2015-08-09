#include "GUI/Button.h"
#include "Serializable.h"
#include "Input.h"

namespace Sentinel { namespace GUI
{
	Button::Button() :
		mState( UP )
	{}

	Button::~Button()
	{}

	void Button::Update( bool isOver )
	{
		if( isOver )
		{
			if( mState == UP )
			{
				mState = OVER;
			}
			
			if( Mouse::Get().IsDown( BUTTON_LEFT ))
			{
				mState = DOWN;

				mActionDown.Activate();
			}

			if( mState == DOWN && Mouse::Get().DidGoUp( BUTTON_LEFT ))
			{
				mState = UP;

				mActionClick.Activate();
			}
		}
		else
		if( mState != UP )
		{
			mState = UP;

			mActionUp.Activate();
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
