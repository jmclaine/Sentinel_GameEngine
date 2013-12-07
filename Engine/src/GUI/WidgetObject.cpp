#pragma once

#include "GUI/WidgetObject.h"
#include "GUI/WidgetWorld.h"
#include "GUI/ModelWidget.h"
#include "GUI/ViewWidget.h"
#include "GUI/ControllerWidget.h"

namespace Sentinel { namespace GUI
{
	WidgetObject::WidgetObject() :
		mWorld( NULL ),
		mModel( NULL ),
		mView( NULL ),
		mController( NULL )
	{}

	WidgetObject::~WidgetObject()
	{
		SAFE_DELETE( mModel );
		SAFE_DELETE( mView );
		SAFE_DELETE( mController );

		TRAVERSE_VECTOR( x, mChild )
		{
			delete mChild[ x ];
		}
	}

	WidgetWorld* WidgetObject::GetWorld()
	{
		return mWorld;
	}

	void WidgetObject::SetWorld( WidgetWorld* world )
	{
		mWorld = world;

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->SetWorld( world );
		}
	}

	ModelWidget* WidgetObject::GetModel()
	{
		return mModel;
	}

	void WidgetObject::SetModel( ModelWidget* widget )
	{
		widget->SetOwner( this );

		mModel = widget;
	}

	ViewWidget* WidgetObject::GetView()
	{
		return mView;
	}

	void WidgetObject::SetView( ViewWidget* widget )
	{
		widget->SetOwner( this );

		mView = widget;
	}

	ControllerWidget* WidgetObject::GetController()
	{
		return mController;
	}

	void WidgetObject::SetController( ControllerWidget* widget )
	{
		widget->SetOwner( this );

		mController = widget;
	}

	/////////////////////////////////////

	void WidgetObject::Startup()
	{
		if( mModel )
			mModel->Startup();

		if( mView )
			mView->Startup();

		if( mController )
			mController->Startup();

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->Startup();
		}
	}

	void WidgetObject::UpdateModel()
	{
		if( mModel )
			mModel->Update();

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->UpdateModel();
		}
	}

	void WidgetObject::UpdateView()
	{
		if( mView )
			mView->Update();

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->UpdateView();
		}
	}

	void WidgetObject::UpdateController()
	{
		if( mController )
			mController->Update();

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->UpdateController();
		}
	}

	void WidgetObject::Shutdown()
	{
		if( mModel )
			mModel->Shutdown();

		if( mView )
			mView->Shutdown();

		if( mController )
			mController->Shutdown();

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->Shutdown();
		}
	}

	/////////////////////////////////////

	void WidgetObject::Save( Archive& archive )
	{
		if( mModel )
			mModel->SerialSave( archive );

		if( mView )
			mView->SerialSave( archive );

		if( mController )
			mController->SerialSave( archive );

		UINT size = (UINT)mChild.size();
		archive.Write( &size );

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->Save( archive );
		}
	}

	void WidgetObject::Load( Archive& archive )
	{
		mModel = (ModelWidget*)SerialRegister::Load( archive );
		if( mModel )
			mModel->Load( archive );
			
		mView = (ViewWidget*)SerialRegister::Load( archive );
		if( mView )
			mView->Load( archive );

		mController = (ControllerWidget*)SerialRegister::Load( archive );
		if( mController )
			mController->Load( archive );

		UINT size;
		archive.Read( &size );

		WidgetObject* obj;
		for( BYTE x = 0; x < size; ++x )
		{
			obj = (WidgetObject*)SerialRegister::Load( archive );
			if( obj )
			{
				AddChild( obj );
				obj->Load( archive );
			}
		}
	}
}}
