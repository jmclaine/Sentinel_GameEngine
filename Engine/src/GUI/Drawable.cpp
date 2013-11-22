#include "GUI/Drawable.h"
#include "Mesh.h"
#include "CameraComponent.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( Drawable );
	DEFINE_SERIAL_CLONE( Drawable );

	Drawable::Drawable() :
		mActionOver( NULL ),
		mIsOver( false )
	{}

	Drawable::~Drawable()
	{}

	void Drawable::Update()
	{
		Widget::PreUpdate();

		// Check state of button.
		//
		if( IsOver() && !mIsOver )
		{
			Over();
		}
		else
		{
			mIsOver = false;
		}

		// Draw mesh.
		//
		mMesh->mMatrixWorld = MATRIX_WVP * mMatrixWorld;
		mMesh->Draw( GAME_WORLD->mRenderer, GAME_WORLD );

		// Update children.
		//
		Widget::PostUpdate();
	}

	void Drawable::Over()
	{
		mIsOver = true;

		Widget::Over();
	}

	///////////////////////////////////

	void Drawable::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		Widget::Save( archive );
	}

	void Drawable::Load( Archive& archive )
	{
		Widget::Load( archive );
	}
}}
