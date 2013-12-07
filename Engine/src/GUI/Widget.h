#pragma once

#include <functional>

#include "Common.h"
#include "Serializable.h"
#include "Archive.h"

namespace Sentinel { namespace GUI
{
	typedef std::function< void() > WidgetFunc;

	class WidgetObject;

	class SENTINEL_DLL Widget : public Serializable
	{
	public:

		enum Type
		{
			MODEL,
			VIEW,
			CONTROLLER,
		};
		
	protected:

		WidgetObject*	mOwner;
		Widget::Type	mType;

	protected:

		Widget();

	public:

		virtual ~Widget();

		//////////////////////////////////

		virtual void	Startup() = 0;

		virtual void	Update() = 0;

		virtual void	Shutdown() = 0;

		//////////////////////////////////

		void			SetOwner( WidgetObject* owner );

		//////////////////////////////////

		virtual void	Save( Archive& archive ) = 0;
		virtual void	Load( Archive& archive ) = 0;
	};
}}
