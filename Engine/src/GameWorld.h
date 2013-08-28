#pragma once
/*
Attached GameObject(s) are automatically shutdown and removed during Shutdown()
*/
#include <vector>
#include <map>

#include "Common.h"
#include "Singleton.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "GameObject.h"

namespace Sentinel
{
	class SENTINEL_DLL GameWorld : public SingletonWindow< GameWorld >
	{
		friend class SingletonWindow< GameWorld >;

	private:

		std::multimap< float, GameObject* >		mAlphaOrder;

		std::vector< CameraComponent* >			mCamera;
		CameraComponent*						mCurrentCamera;

		std::vector< LightComponent* >			mLight;

		std::vector< GameObject* >				mGameObject;
		
	private:

		GameWorld();
		~GameWorld();

	public:

		static GameWorld*	Load( const std::string& mapName, void* hWnd = NULL );
		
		void				Startup();

		void				Update();
		void				UpdateController();
		void				UpdatePhysics();
		void				UpdateDrawable();
		void				UpdateComponents();

		void				Shutdown();

		//////////////////////////////

		// Returns entity.
		//
		GameObject*			AddGameObject( GameObject* entity, const std::string& name );

		// Removes and returns entity.
		//
		GameObject*			RemoveGameObject( GameObject* entity );

		//////////////////////////////

		void				SetCamera( CameraComponent* camera );

		CameraComponent*	GetCamera( int index = -1 );	// -1 = mCurrentCamera
		LightComponent*		GetLight( UINT index );
		GameObject*			GetGameObject( UINT index );
	};
}
