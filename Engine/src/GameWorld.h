#pragma once

#include <vector>

#include "Singleton.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "GameObject.h"

namespace Sentinel
{
	class GameWorld : public Singleton< GameWorld >
	{
	public:

		CameraComponent*						mCamera;
		std::vector< LightComponent* >			mLight;

		std::vector< GameObject* >				mGameObject;
		
		//////////////////////////////

		GameWorld();
		
		void Startup( const std::string& mapName );

		void Update();

		void Shutdown();

		//////////////////////////////

		GameObject* AddGameObject( GameObject* entity, const std::string& name );

		void RemoveGameObject( GameObject* entity );
	};
}
