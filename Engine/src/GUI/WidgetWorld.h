#pragma once

#include <vector>

#include "Common.h"
#include "Matrix4f.h"
#include "Archive.h"

namespace Sentinel 
{ 
	class SpriteSystem;
	class FontSystem;
	class WindowInfo;
	class GameWorld;

namespace GUI
{
	class WidgetObject;

	class SENTINEL_DLL WidgetWorld
	{
	private:

		std::vector< WidgetObject* > mWidgetObject;

	public:

		Matrix4f			mMatrixWVP;

		WindowInfo*			mWindowInfo;
		GameWorld*			mGameWorld;
		SpriteSystem*		mSpriteSystem;
		FontSystem*			mFontSystem;
		
	public:

		WidgetWorld();
		~WidgetWorld();

		void				Release();

		// All associated WidgetObjects and Widgets also Startup().
		//
		void				Startup();

		// All associated WidgetObjects and Widgets also Update().
		//
		void				Update();

		// All associated WidgetObjects and Widgets also Shutdown().
		//
		void				Shutdown();

		// Returns entity.
		//
		WidgetObject*		AddWidgetObject( WidgetObject* entity );

		// Removes and returns entity.
		// Entity must be deleted unless attached to another WidgetObject.
		//
		WidgetObject*		RemoveWidgetObject( WidgetObject* entity );

		// Returns WidgetObject at index.
		//
		WidgetObject*		GetWidgetObject( UINT index );
		UINT				NumWidgetObjects();

		//////////////////////////////////////////////////////////////////

		void				Save( Archive& archive );
		void				Load( Archive& archive );
	};
}}
