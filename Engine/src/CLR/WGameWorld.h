#pragma once

#include "GameWorld.h"
#include "WGameObject.h"
#include "WCameraComponent.h"
#include "WLightComponent.h"

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
		static void					UpdateTransform();
		static void					UpdateComponents();
		static void					UpdateDrawable();

		static void					Shutdown();

		//////////////////////////////

		static WGameObject^			AddGameObject( WGameObject^ entity );
		static WGameObject^			AddGameObject( WGameObject^ entity, System::String^ name );

		static WGameObject^			RemoveGameObject( WGameObject^ entity );

		static WGameObject^			GetGameObject( int index );
		static int					NumGameObjects();

		//////////////////////////////

		static void					SetCamera( WCameraComponent^ camera );

		static WCameraComponent^	GetCamera( int index );	// -1 = mCurrentCamera
		static WLightComponent^		GetLight( UINT index );
	};
}}
