#include "GUI/Widget.h"
#include "Renderer.h"
#include "Quatf.h"

namespace Sentinel { namespace GUI
{
	Root::Root() :
		mIsVisible( true )
	{}

	Root::~Root()
	{}

	void Root::Update()
	{
		if( mIsVisible )
		{
			TRAVERSE_VECTOR( x, mChild )
			{
				mChild[ x ]->PreUpdate();
			}

			TRAVERSE_VECTOR( x, mChild )
			{
				mChild[ x ]->Update();
			}

			TRAVERSE_VECTOR( x, mChild )
			{
				mChild[ x ]->PostUpdate();
			}
		}
	}

	//////////////////////////////////

	void Root::Save( Archive& archive )
	{
		archive.Write( &mIsVisible );

		UINT size = mChild.size();
		archive.Write( &size );

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->Save( archive );
		}
	}

	void Root::Load( Archive& archive )
	{
		archive.Read( &mIsVisible );

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

	/////////////////////////////////////////////////////////////////////

	WindowInfo*		Widget::WINDOW_INFO		= NULL;
	GameWorld*		Widget::GAME_WORLD		= NULL;
	SpriteSystem*	Widget::SPRITE_SYSTEM	= NULL;
	FontSystem*		Widget::FONT_SYSTEM		= NULL;
	Matrix4f		Widget::MATRIX_WVP		= Matrix4f::IDENTITY;
	
	//////////////////////////////////

	Widget::Widget() :
		mScale( Vector3f( 1, 1, 1 )),
		mPositionToWindowX( false ),
		mPositionToWindowY( false ),
		mScaleToWindowX( false ),
		mScaleToWindowY( false ),
		mIsOver( false ),
		mActionOver( NULL )
	{}

	Widget::~Widget()
	{}

	void Widget::PreUpdate()
	{
		// Position widget.
		//
		Vector3f pos   = mPosition;
		Vector3f scale = mScale;

		if( mPositionToWindowX )
			pos.x *= WINDOW_INFO->WidthRatio();
		
		if( mPositionToWindowY )
			pos.y *= WINDOW_INFO->HeightRatio();

		if( mScaleToWindowX )
			scale.x *= WINDOW_INFO->WidthRatio();
		
		if( mScaleToWindowY )
			scale.y *= WINDOW_INFO->HeightRatio();

		pos.x += mMargin.left;
		pos.y += mMargin.top;

		scale.x += mMargin.right;
		scale.y += mMargin.bottom;

		mMatrixWorld.World( pos, Quatf( mRotation ), scale );

		// Check for mouse being over the widget.
		//
		POINT mousePos = Mouse::Get().GetPosition( (HWND)WINDOW_INFO->Handle() );

		Matrix4f matRot;
		matRot.Rotate( -mRotation );
		Vector3f v = matRot.Transform( Vector3f( mousePos.x-pos.x, mousePos.y-pos.y, 0 ));

		if( v.x >= 0 && v.y >= 0 &&
			v.x <= scale.x && v.y <= scale.y )
		{
			if( !mIsOver )
			{
				mIsOver = true;
				
				if( mActionOver )
					mActionOver();
			}
		}
		else
		{
			mIsOver = false;
		}

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->PreUpdate();
		}
	}

	void Widget::Update()
	{
		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->Update();
		}
	}

	void Widget::PostUpdate()
	{
		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->PostUpdate();
		}
	}

	void Widget::Save( Archive& archive )
	{
		archive.Write( mPosition.Ptr(), ar_sizeof( mPosition ));
		archive.Write( mRotation.Ptr(), ar_sizeof( mRotation ));
		archive.Write( mScale.Ptr(),    ar_sizeof( mScale ));

		archive.Write( mColor.Ptr(),    ar_sizeof( mColor ));

		archive.Write( &mPositionToWindowX );
		archive.Write( &mPositionToWindowY );

		archive.Write( &mScaleToWindowX );
		archive.Write( &mScaleToWindowY );

		Root::Save( archive );
	}

	void Widget::Load( Archive& archive )
	{
		archive.Read( mPosition.Ptr(), ar_sizeof( mPosition ));
		archive.Read( mRotation.Ptr(), ar_sizeof( mRotation ));
		archive.Read( mScale.Ptr(),    ar_sizeof( mScale ));

		archive.Read( mColor.Ptr(),    ar_sizeof( mColor ));

		archive.Read( &mPositionToWindowX );
		archive.Read( &mPositionToWindowY );

		archive.Read( &mScaleToWindowX );
		archive.Read( &mScaleToWindowY );

		Root::Load( archive );
	}
}}
