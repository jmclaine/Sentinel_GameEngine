#include "GUI/Widget.h"
#include "Renderer.h"
#include "Quatf.h"

namespace Sentinel { namespace GUI
{
	Widget::Widget() :
		mOwner( NULL )
	{}

	Widget::~Widget()
	{}

	void Widget::SetOwner( WidgetObject* owner )
	{
		mOwner = owner;
	}
}}
