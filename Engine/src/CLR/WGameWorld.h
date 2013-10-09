#pragma once

#include "GameWorld.h"
#include "WGameObject.h"
#include "WCameraComponent.h"
#include "WLightComponent.h"
#include "WArchive.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	public ref class WGameWorld
	{
	public:

		static void					Create();
		static void					Release();
		
		static void					Save( WArchive^ archive );
		static void					Load( WArchive^ archive );

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

		static WGameObject^			GetGameObject( UINT index );
		static int					NumGameObjects();

		//////////////////////////////

		static void					SetCamera( WCameraComponent^ camera );

		static WCameraComponent^	GetCamera( int index );	// -1 = mCurrentCamera
		static WLightComponent^		GetLight( UINT index );
	};
}}
