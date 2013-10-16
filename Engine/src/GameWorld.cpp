#include "GameWorld.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "Timing.h"
#include "Archive.h"
#include "PhysicsSystem.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "ModelManager.h"

namespace Sentinel
{
	GameWorld::GameWorld() :
		mCurrentCamera( NULL ),
		mRenderer( NULL ),
		mPhysicsSystem( NULL ),
		mTiming( NULL ),
		mTextureManager( NULL ),
		mShaderManager( NULL ),
		mMeshManager( NULL ),
		mModelManager( NULL )
	{}

	GameWorld::~GameWorld()
	{
		Release();
	}

	void GameWorld::Release()
	{
		TRAVERSE_VECTOR( x, mGameObject )
			SAFE_DELETE( mGameObject[ x ] );

		mGameObject.clear();
		mAlphaOrder.clear();
		mCamera.clear();
		mLight.clear();

		mCurrentCamera = NULL;
	}

	void GameWorld::Save( Archive& archive )
	{
		UINT size = mGameObject.size();
		archive.Write( &size );

		TRAVERSE_VECTOR( x, mGameObject )
			mGameObject[ x ]->Save( archive );
	}

	void GameWorld::Load( Archive& archive )
	{
		Release();

		UINT size = 0;
		archive.Read( &size );

		GameObject* obj;
		for( UINT x = 0; x < size; ++x )
		{
			obj = (GameObject*)SerialRegister::Load( archive );
			AddGameObject( obj );
			obj->Load( archive );
		}
	}

	void GameWorld::Startup()
	{
		TRAVERSE_VECTOR( x, mGameObject )
		{
			GameObject* obj = mGameObject[ x ];

			CameraComponent* camera = (CameraComponent*)obj->FindComponent( GameComponent::CAMERA );
		
			if( camera )
				mCamera.push_back( camera );
			
			LightComponent* light = (LightComponent*)obj->FindComponent( GameComponent::LIGHT );
			
			if( light )
				mLight.push_back( light );
		}

		if( !mCurrentCamera )
			mCurrentCamera = (!mCamera.empty()) ? mCamera[ 0 ] : NULL;

		// Create a default light if none exist.
		//
		if( mLight.empty() && mCurrentCamera )
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

	void GameWorld::UpdateController()
	{
		TRAVERSE_VECTOR( x, mGameObject )
			mGameObject[ x ]->UpdateController();
	}

	void GameWorld::UpdatePhysics()
	{
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
		if( mCurrentCamera )
		{
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
			// TODO: Raycast from center screen to AABB for more accuracy.
			//
			Vector3f camPos = mCurrentCamera->GetTransform()->mPosition;
		
			mAlphaOrder.clear();
			TRAVERSE_VECTOR( x, mGameObject )
			{
				TransformComponent* transform = (TransformComponent*)mGameObject[ x ]->FindComponent( GameComponent::TRANSFORM );
			
				if( transform )
					mAlphaOrder.insert( std::pair< float, GameObject* >( -(camPos - transform->mPosition).LengthSquared(), mGameObject[ x ] ));
			}

			// Update and Render Meshes.
			//
			TRAVERSE_LIST( it, mAlphaOrder )
				(*it).second->UpdateDrawable();
		}
	}

	void GameWorld::Shutdown()
	{
		TRAVERSE_VECTOR( x, mGameObject )
			mGameObject[ x ]->Shutdown();
	}

	/////////////////////////////////

	GameObject* GameWorld::AddGameObject( GameObject* entity )
	{
		if( entity )
		{
			TRAVERSE_VECTOR( x, mGameObject )
			{
				if( mGameObject[ x ] == entity )
					return entity;
			}

			if( entity->mParent )
				entity->mParent->RemoveChild( entity );

			entity->mWorld = this;
		
			mGameObject.push_back( entity );
		}

		return entity;
	}

	GameObject* GameWorld::AddGameObject( GameObject* entity, const char* name )
	{
		if( entity )
		{
			entity->mName = name;

			return AddGameObject( entity );
		}

		return entity;
	}

	GameObject* GameWorld::RemoveGameObject( GameObject* entity )
	{
		TRAVERSE_LIST( it, mGameObject )
		{
			if( (*it) == entity )
			{
				mGameObject.erase( it );
				return entity;
			}
		}
		
		return entity;
	}

	GameObject* GameWorld::GetGameObject( UINT index )
	{
		_ASSERT( index < mGameObject.size() );

		return mGameObject[ index ];
	}

	UINT GameWorld::NumGameObjects()
	{
		return mGameObject.size();
	}

	/////////////////////////////////

	// -1 = mCurrentCamera
	CameraComponent* GameWorld::GetCamera( int index )
	{
		_ASSERT( index < (int)mCamera.size() && index >= -1 );

		return (index > -1) ? mCamera[ index ] : mCurrentCamera;
	}

	void GameWorld::SetCamera( CameraComponent* camera )
	{
		_ASSERT( camera );

		mCurrentCamera = camera;
	}

	UINT GameWorld::NumCameras()
	{
		return mCamera.size();
	}

	LightComponent* GameWorld::GetLight( UINT index )
	{
		_ASSERT( index < mLight.size() );

		return mLight[ index ];
	}

	UINT GameWorld::NumLights()
	{
		return mLight.size();
	}
}
