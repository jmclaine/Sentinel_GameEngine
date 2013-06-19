#include "PhysicsSystem.h"
#include "GameWorld.h"
#include "Util.h"

namespace Sentinel
{
	GameWorld::GameWorld()
	{
		mCamera = NULL;
	}

	void GameWorld::Startup( const std::string& mapName )
	{
		// No map file format to load...yet.
		
		// For now, set the size of the Quadtree to some default value.

		TRAVERSE_VECTOR( x, mGameObject )
		{
			GameObject* obj = mGameObject[ x ];

			obj->Startup();

			if( !mCamera )
			{
				CameraComponent* camera;
				camera = (CameraComponent*)obj->FindComponent( COMPONENT_CAMERA );

				if( camera )
					mCamera = camera;
			}
			
			obj->FindComponent( COMPONENT_LIGHT, mLight );
		}
	}

	void GameWorld::Update()
	{
		// Update Controllers.
		//
		TRAVERSE_VECTOR( x, mGameObject )
		{
			mGameObject[ x ]->UpdateController();
		}

		// Update Physics.
		//
		PhysicsSystem::Inst()->Update();

		TRAVERSE_VECTOR( x, mGameObject )
		{
			mGameObject[ x ]->UpdatePhysics();
		}

		// Update other components.
		//
		TRAVERSE_VECTOR( x, mGameObject )
		{
			mGameObject[ x ]->UpdateComponents();
		}

		// Update and Render Meshes.
		//
		TRAVERSE_VECTOR( x, mGameObject )
		{
			mGameObject[ x ]->UpdateDrawable();
		}
	}

	void GameWorld::Shutdown()
	{
		TRAVERSE_VECTOR( x, mGameObject )
		{
			mGameObject[ x ]->Shutdown();

			SAFE_DELETE( mGameObject[ x ] );
		}

		mGameObject.clear();
	}

	/////////////////////////////////

	GameObject* GameWorld::AddGameObject( GameObject* entity, const std::string& name )
	{
		entity->mName = name;

		mGameObject.push_back( entity );

		return entity;
	}

	void GameWorld::RemoveGameObject( GameObject* entity )
	{
		TRAVERSE_LIST( it, mGameObject )
			if( (*it) == entity )
			{
				entity->Shutdown();
				delete entity;
				mGameObject.erase( it );
			}
	}
}
