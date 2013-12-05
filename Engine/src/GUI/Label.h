#pragma once

#include "GUI/Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL Label : public Widget
	{
	public:

		std::string		mText;

		//////////////////////////////////

		Label( const std::string& text );
		virtual ~Label();

		virtual void Update();
	};
}}
