#include "GUI/Widget.h"
#include "Renderer.h"

namespace Sentinel { namespace GUI
{
	Renderer* Widget::RENDERER				= NULL;
	std::shared_ptr< Sprite > Widget::SKIN	= NULL;

	void Widget::DoNothing()
	{}

	//////////////////////////////////

	Widget::Widget()
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
		mDimension.left = mParent->mDimension.left + static_cast< float >(pos.x);
		mDimension.top  = mParent->mDimension.top  + static_cast< float >(pos.y);
	}

	Point2i Widget::GetPosition()
	{
		return Point2i( static_cast< int >(mDimension.left), static_cast< int >(mDimension.top) );
	}
}}
