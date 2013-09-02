#include "GameWorld.h"
#include "PhysicsSystem.h"

namespace Sentinel
{
	GameWorld::GameWorld()
	{
		mCurrentCamera = NULL;
	}

	GameWorld::~GameWorld()
	{
		Shutdown();
	}

	GameWorld* GameWorld::Load( const std::string& mapName, void* hWnd )
	{
		GameWorld* world = GameWorld::Inst( hWnd );

		// No map file format to load...yet.

		return world;
	}

	void GameWorld::Startup()
	{
		TRAVERSE_VECTOR( x, mGameObject )
		{
			GameObject* obj = mGameObject[ x ];

			CameraComponent* camera = (CameraComponent*)obj->FindComponent( COMPONENT_CAMERA );
		
			if( camera )
				mCamera.push_back( camera );
			
			LightComponent* light = (LightComponent*)obj->FindComponent( COMPONENT_LIGHT );
			
			if( light )
				mLight.push_back( light );
		}

		if( !mCurrentCamera )
			mCurrentCamera = (!mCamera.empty()) ? mCamera[ 0 ] : NULL;

		// Create a default light if none exist.
		//
		if( mLight.empty() )
		{
			LightComponent* light = new LightComponent();
			light->mAttenuation = Vector4f( 1, 1, 1, 2000 );
			light->mColor		= ColorRGBA( 1, 1, 1, 1 );

			TransformComponent* transform = new TransformComponent();
			transform->mPosition = Vector3f( 0, 250, 0 );

			GameObject* obj = AddGameObject( new GameObject(), "Default_Light" );
			obj->AttachComponent( transform, "Transform" );
			obj->AttachComponent( light, "Light" );

			mLight.push_back( light );
		}

		TRAVERSE_VECTOR( x, mGameObject )
			mGameObject[ x ]->Startup();
	}

	void GameWorld::Update()
	{
		UpdateController();
		UpdatePhysics();
		UpdateTransform();
		UpdateComponents();
		UpdateDrawable();
	}

	void GameWorld::UpdateController()
	{
		TRAVERSE_VECTOR( x, mGameObject )
			mGameObject[ x ]->UpdateController();
	}

	void GameWorld::UpdatePhysics()
	{
		PhysicsSystem::Inst()->Update();

		TRAVERSE_VECTOR( x, mGameObject )
			mGameObject[ x ]->UpdatePhysics();
	}

	void GameWorld::UpdateTransform()
	{
		TRAVERSE_VECTOR( x, mGameObject )
			mGameObject[ x ]->UpdateTransform();
	}

	void GameWorld::UpdateComponents()
	{
		TRAVERSE_VECTOR( x, mGameObject )
			mGameObject[ x ]->UpdateComponents();
	}

	void GameWorld::UpdateDrawable()
	{
		_ASSERT( mCurrentCamera );

		TRAVERSE_VECTOR( x, mCamera )
			mCamera[ x ]->Update();

		TRAVERSE_VECTOR( x, mLight )
			mLight[ x ]->Update();

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
			(*it).second->UpdateDrawable();
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

	GameObject* GameWorld::RemoveGameObject( GameObject* entity )
	{
		TRAVERSE_LIST( it, mGameObject )
			if( (*it) == entity )
				mGameObject.erase( it );
		
		return entity;
	}

	/////////////////////////////////

	void GameWorld::SetCamera( CameraComponent* camera )
	{
		_ASSERT( camera );

		mCurrentCamera = camera;
	}

	// -1 = mCurrentCamera
	CameraComponent* GameWorld::GetCamera( int index )
	{
		_ASSERT( index < (int)mCamera.size() && index >= -1 );

		return (index > -1) ? mCamera[ index ] : mCurrentCamera;
	}

	LightComponent* GameWorld::GetLight( UINT index )
	{
		_ASSERT( index < mLight.size() );

		return mLight[ index ];
	}

	GameObject* GameWorld::GetGameObject( UINT index )
	{
		_ASSERT( index < mGameObject.size() );

		return mGameObject[ index ];
	}
}
