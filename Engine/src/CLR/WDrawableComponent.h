#pragma once
/*
There is no open functionality in DrawableComponent.
Creating this ensures derived classes can be casted
into a familiar class name, and prepares the class
for future improvements.
*/
#include "WGameObject.h"

namespace Sentinel { namespace Components
{
	public ref class WDrawableComponent : public WGameComponent
	{};
}}
