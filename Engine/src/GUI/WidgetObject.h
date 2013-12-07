#pragma once

#include "Common.h"
#include "ListNode.h"
#include "Util.h"
#include "Input.h"
#include "Archive.h"

namespace Sentinel { namespace GUI
{
	class WidgetWorld;
	class ModelWidget;
	class ViewWidget;
	class ControllerWidget;

	class SENTINEL_DLL WidgetObject : public ListNode< WidgetObject >
	{
	protected:

		WidgetWorld*		mWorld;

		ModelWidget*		mModel;
		ViewWidget*			mView;
		ControllerWidget*	mController;

	public:

		std::string			mName;

		/////////////////////////////////////

		WidgetObject();
		virtual ~WidgetObject();

		WidgetWorld*		GetWorld();
		void				SetWorld( WidgetWorld* world );

		ModelWidget*		GetModel();
		void				SetModel( ModelWidget* widget );

		ViewWidget*			GetView();
		void				SetView( ViewWidget* widget );

		ControllerWidget*	GetController();
		void				SetController( ControllerWidget* widget );

		/////////////////////////////////////

		virtual void		Startup();

		virtual void		UpdateModel();
		virtual void		UpdateView();
		virtual void		UpdateController();

		virtual void		Shutdown();

		/////////////////////////////////////

		void				Save( Archive& archive );
		void				Load( Archive& archive );
	};
}}
