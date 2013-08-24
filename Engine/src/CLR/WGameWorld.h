#pragma once

#include "GameWorld.h"
#include "WGameObject.h"
#include "WCameraComponent.h"
//#include "WLightComponent.h"

namespace Sentinel { namespace Components
{
	public ref class WGameWorld
	{
	public:

		static void					Load( System::String^ mapName );
		static void					Load( System::String^ mapName, IntPtr hWnd );

		static void					Startup();

		static void					Update();
		static void					UpdateController();
		static void					UpdatePhysics();
		static void					UpdateDrawable();
		static void					UpdateComponents();

		static void					Shutdown();

		//////////////////////////////

		static RGameObject^			AddGameObject( WGameObject^ entity, System::String^ name );

		static RGameObject^			RemoveGameObject( RGameObject^ entity );

		//////////////////////////////

		static void					SetCamera( UINT index );

		static WCameraComponent^	GetCamera( int index );	// -1 = mCurrentCamera
		//static WLightComponent^		GetLight( UINT index );
		static RGameObject^			GetGameObject( UINT index );
	};
}}
