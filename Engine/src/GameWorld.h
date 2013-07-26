#pragma once

#include <vector>
#include <map>

#include "Singleton.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "GameObject.h"

namespace Sentinel
{
	class GameWorld : public SingletonThreaded< GameWorld >
	{
		friend class SingletonThreaded< GameWorld >;

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
		
		void				Startup( const std::string& mapName );

		void				Update();

		void				Shutdown();

		//////////////////////////////

		GameObject*			AddGameObject( GameObject* entity, const std::string& name );

		void				RemoveGameObject( GameObject* entity );

		//////////////////////////////

		void				SetCamera( UINT index );

		CameraComponent*	GetCamera( int index = -1 );	// -1 = mCurrentCamera
		LightComponent*		GetLight( UINT index );
		GameObject*			GetGameObject( UINT index );
	};
}
