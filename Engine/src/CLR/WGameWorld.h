#pragma once

#include "GameWorld.h"
#include "WGameObject.h"
//#include "WCameraComponent.h"
//#include "WLightComponent.h"

namespace Sentinel { namespace Components
{
	public ref class WGameWorld
	{
	public:

		static void					Startup( System::String^ mapName );

		static void					Update();
		static void					UpdateController();
		static void					UpdatePhysics();
		static void					UpdateDrawable();
		static void					UpdateComponents();

		static void					Shutdown();

		//////////////////////////////

		static WGameObject^			AddGameObject( WGameObject^ entity, System::String^ name );

		static void					RemoveGameObject( WGameObject^ entity );

		//////////////////////////////

		static void					SetCamera( UINT index );

		//static WCameraComponent^	GetCamera( int index = -1 );	// -1 = mCurrentCamera
		//static WLightComponent^		GetLight( UINT index );
		static WGameObject^			GetGameObject( UINT index );
	};
}}
