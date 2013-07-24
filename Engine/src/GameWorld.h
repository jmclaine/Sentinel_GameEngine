#pragma once

#include <vector>
#include <map>

#include "Singleton.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "GameObject.h"

namespace Sentinel
{
	class GameWorld : public Singleton< GameWorld >
	{
	private:

		std::multimap< float, GameObject* >		mAlphaOrder;

		std::vector< CameraComponent* >			mCamera;

	public:

		CameraComponent*						mCurrentCamera;

		std::vector< LightComponent* >			mLight;

		std::vector< GameObject* >				mGameObject;
		
		//////////////////////////////

		GameWorld();
		~GameWorld();
		
		void Startup( const std::string& mapName );

		void Update();

		void Shutdown();

		//////////////////////////////

		GameObject* AddGameObject( GameObject* entity, const std::string& name );

		void RemoveGameObject( GameObject* entity );

		//////////////////////////////

		void SetCamera( UINT index );
	};
}
