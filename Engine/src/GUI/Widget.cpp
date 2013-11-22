#include "GUI/Widget.h"
#include "Renderer.h"
#include "Quatf.h"

namespace Sentinel { namespace GUI
{
	HWND			Widget::WINDOW		= NULL;
	GameWorld*		Widget::GAME_WORLD	= NULL;
	Matrix4f		Widget::MATRIX_WVP	= Matrix4f::IDENTITY;
	
	//////////////////////////////////

	Widget::Widget() :
		mScale( Vector3f( 1, 1, 1 )),
		mIsVisible( true ),
		mPositionToWindow( false ),
		mScaleToWindow( false ),
		mActionOver( NULL )
	{}

	Widget::~Widget()
	{}

	void Widget::PreUpdate()
	{
		Vector3f pos   = mPosition;
		Vector3f scale = mScale;

		if( mPositionToWindow )
		{
			pos.x *= GAME_WORLD->mRenderer->GetWindow()->WidthRatio();
			pos.y *= GAME_WORLD->mRenderer->GetWindow()->HeightRatio();
		}

		if( mScaleToWindow )
		{
			scale.x *= GAME_WORLD->mRenderer->GetWindow()->WidthRatio();
			scale.y *= GAME_WORLD->mRenderer->GetWindow()->HeightRatio();
		}

		mMatrixWorld.World( pos, Quatf( mRotation ), scale );
	}

	void Widget::Update()
	{
		PreUpdate();

		// ...

		PostUpdate();
	}

	void Widget::PostUpdate()
	{
		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->Update();
		}
	}

	bool Widget::IsOver()
	{
		POINT mousePos = Mouse::Get().GetPosition( WINDOW );

		Vector3f pos   = mPosition;
		Vector3f scale = mScale;

		if( mPositionToWindow )
		{
			pos.x *= GAME_WORLD->mRenderer->GetWindow()->WidthRatio();
			pos.y *= GAME_WORLD->mRenderer->GetWindow()->HeightRatio();
		}

		if( mScaleToWindow )
		{
			scale.x *= GAME_WORLD->mRenderer->GetWindow()->WidthRatio();
			scale.y *= GAME_WORLD->mRenderer->GetWindow()->HeightRatio();
		}
		
		if( mousePos.x >= pos.x && mousePos.y >= pos.y &&
			mousePos.x <= pos.x + scale.x && mousePos.y <= pos.y + scale.y )
			return true;

		return false;
	}

	void Widget::Over()
	{
		if( mActionOver )
			mActionOver();
	}

	void Widget::Save( Archive& archive )
	{
		archive.Write( mPosition.Ptr(), ar_sizeof( mPosition ));
		archive.Write( mRotation.Ptr(), ar_sizeof( mRotation ));
		archive.Write( mScale.Ptr(),    ar_sizeof( mScale ));

		archive.Write( mColor.Ptr(),    ar_sizeof( mColor ));

		archive.Write( &mIsVisible );
		archive.Write( &mPositionToWindow );
		archive.Write( &mScaleToWindow );

		UINT size = mChild.size();
		archive.Write( &size );

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->Save( archive );
		}
	}

	void Widget::Load( Archive& archive )
	{
		archive.Read( mPosition.Ptr(), ar_sizeof( mPosition ));
		archive.Read( mRotation.Ptr(), ar_sizeof( mRotation ));
		archive.Read( mScale.Ptr(),    ar_sizeof( mScale ));

		archive.Read( mColor.Ptr(),    ar_sizeof( mColor ));

		archive.Read( &mIsVisible );
		archive.Read( &mPositionToWindow );
		archive.Read( &mScaleToWindow );

		UINT size = 0;
		archive.Read( &size );

		Widget* widget;
		for( UINT x = 0; x < size; ++x )
		{
			widget = (Widget*)SerialRegister::Load( archive );
			AddChild( widget );
			widget->Load( archive );
		}
	}
}}
