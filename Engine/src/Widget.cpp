#include "GUI/Widget.h"
#include "Renderer.h"

namespace Sentinel { namespace GUI
{
	SpriteSystem* Widget::SPRITE_SYSTEM = NULL;
	
	//////////////////////////////////

	Widget::Widget() :
		mZIndex( 0 ),
		mIsVisible( true )
	{}

	Widget::~Widget()
	{}

	void Widget::Update()
	{
		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->Update();
		}
	}

	bool Widget::IsOver()
	{
		POINT pos = Mouse::Get().GetPosition();

		if( pos.x >= mDimension.left  && pos.y >= mDimension.top &&
			pos.x <= mDimension.right && pos.y <= mDimension.bottom )
			return true;

		return false;
	}

	void Widget::SetPosition( const Point2i& pos )
	{
		mDimension.left = static_cast< float >(pos.x);
		mDimension.top  = static_cast< float >(pos.y);

		if( mParent )
		{
			mDimension.left += mParent->mDimension.left;
			mDimension.top  += mParent->mDimension.top;
		}
	}

	Point2i Widget::GetPosition()
	{
		return Point2i( static_cast< int >(mDimension.left), static_cast< int >(mDimension.top) );
	}
}}
