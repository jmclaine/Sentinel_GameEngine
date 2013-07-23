#include "GameWorld.h"

namespace Sentinel
{
	GameWorld::GameWorld()
	{}

	void GameWorld::Startup( const std::string& mapName )
	{
		// No map file format to load...yet.

		TRAVERSE_VECTOR( x, mGameObject )
		{
			GameObject* obj = mGameObject[ x ];

			obj->Startup();

			CameraComponent* camera = (CameraComponent*)obj->FindComponent( COMPONENT_CAMERA );
		
			if( camera )
				mCamera.push_back( camera );
			
			LightComponent* light = (LightComponent*)obj->FindComponent( COMPONENT_LIGHT );
			
			if( light )
				mLight.push_back( light );
		}

		if( !mCamera.empty() )
			mCurrentCamera = mCamera[ 0 ];	// assume first camera created is Main Camera
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

		// Meshes may contain alpha values.
		// Put them in order.
		//
		// Use distance from camera to center.
		// This will not be 100% accurate, but is faster than trying to be perfect.
		//
		Vector3f camPos = mCurrentCamera->GetTransform()->mPosition;
		
		mAlphaOrder.clear();
		TRAVERSE_VECTOR( x, mGameObject )
		{
			TransformComponent* transform = (TransformComponent*)mGameObject[ x ]->FindComponent( COMPONENT_TRANSFORM );
			
			if( transform )
				mAlphaOrder.insert( std::pair< float, GameObject* >( -(camPos - transform->mPosition).LengthSquared(), mGameObject[ x ] ));
		}

		// Update and Render Meshes.
		//
		TRAVERSE_LIST( it, mAlphaOrder )
		{
			(*it).second->UpdateDrawable();
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
