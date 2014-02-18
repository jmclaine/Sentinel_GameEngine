#include "GameWorld.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "PointLightComponent.h"
#include "Archive.h"
#include "Timing.h"
#include "PhysicsSystem.h"
#include "AudioSystem.h"
#include "SpriteSystem.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"
#include "SpriteManager.h"
#include "MeshManager.h"
#include "ModelManager.h"

namespace Sentinel
{
	GameWorld::GameWorld() :
		mCurrentCamera( NULL ),
		mRenderer( NULL ),
		mTiming( NULL ),
		mPhysicsSystem( NULL ),
		mAudioSystem( NULL ),
		mSpriteSystem( NULL ),
		mTextureManager( NULL ),
		mShaderManager( NULL ),
		mSpriteManager( NULL ),
		mMeshManager( NULL ),
		mModelManager( NULL ),
		mSoundManager( NULL )
	{}

	GameWorld::~GameWorld()
	{
		Release();

		SAFE_DELETE( mTiming );
		SAFE_DELETE( mPhysicsSystem );
		SAFE_DELETE( mAudioSystem );
		SAFE_DELETE( mSpriteSystem );
		
		SAFE_DELETE( mTextureManager );
		SAFE_DELETE( mShaderManager );
		SAFE_DELETE( mMaterialManager );
		SAFE_DELETE( mSpriteManager );
		SAFE_DELETE( mMeshManager );
		SAFE_DELETE( mModelManager );
		SAFE_DELETE( mSoundManager );
	}

	void GameWorld::Release()
	{
		TRAVERSE_VECTOR( x, mGameObject )
			delete mGameObject[ x ];

		mGameObject.clear();
		mAlphaOrder.clear();
		mCamera.clear();
		mLight.clear();

		mCurrentCamera = NULL;
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
		if( mPhysicsSystem && mTiming )
			mPhysicsSystem->Update( mTiming->DeltaTime() );

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

	void GameWorld::UpdateLight()
	{
		TRAVERSE_VECTOR( x, mLight )
			mLight[ x ]->Update();
	}

	void GameWorld::UpdateDrawable()
	{
		if( mCurrentCamera )
		{
			mCurrentCamera->Update();

			// Meshes may contain alpha values.
			// Put them in order.
			//
			// Use distance from camera to center.
			// This will not be 100% accurate, but is faster than trying to be perfect.
			//
			// TODO: Raycast from center screen to AABB for more accuracy.
			//
			/*
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
			//*/

			TRAVERSE_VECTOR( x, mGameObject )
				mGameObject[ x ]->UpdateDrawable();
		}
	}

	void GameWorld::Shutdown()
	{
		TRAVERSE_VECTOR( x, mGameObject )
			mGameObject[ x ]->Shutdown();
	}

	/////////////////////////////////////////////////////

	GameObject* GameWorld::AddGameObject( GameObject* obj )
	{
		if( obj )
		{
			TRAVERSE_VECTOR( x, mGameObject )
			{
				if( mGameObject[ x ] == obj )
					return obj;
			}

			if( obj->GetParent() )
				obj->GetParent()->RemoveChild( obj );

			obj->SetWorld( this );
		
			mGameObject.push_back( obj );

			////////////////////////////////

			CameraComponent* camera = (CameraComponent*)obj->FindComponent( GameComponent::CAMERA );
		
			if( camera )
				AddCamera( camera );
			
			LightComponent* light = (LightComponent*)obj->FindComponent( GameComponent::LIGHT );
			
			if( light )
				AddLight( light );
		}

		return obj;
	}

	GameObject* GameWorld::AddGameObject( GameObject* obj, const char* name )
	{
		if( obj )
		{
			obj->mName = name;

			return AddGameObject( obj );
		}

		return obj;
	}

	GameObject* GameWorld::RemoveGameObject( GameObject* obj )
	{
		TRAVERSE_LIST( it, mGameObject )
		{
			if( (*it) == obj )
			{
				mGameObject.erase( it );

				obj->SetWorld( NULL );

				return obj;
			}
		}
		
		return obj;
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

	/////////////////////////////////////////////////////

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

	void GameWorld::AddCamera( CameraComponent* camera )
	{
		bool isDup = false;
		TRAVERSE_VECTOR( x, mCamera )
		{
			if( mCamera[ x ] == camera )
			{
				isDup = true;
				break;
			}
		}

		if( !isDup )
			mCamera.push_back( camera );
	}

	/////////////////////////////////////////////////////

	LightComponent* GameWorld::GetLight( UINT index )
	{
		_ASSERT( index < mLight.size() );

		return mLight[ index ];
	}

	UINT GameWorld::NumLights()
	{
		return mLight.size();
	}

	void GameWorld::AddLight( LightComponent* light )
	{
		bool isDup = false;
		TRAVERSE_VECTOR( x, mLight )
		{
			if( mLight[ x ] == light )
			{
				isDup = true;
				break;
			}
		}

		if( !isDup )
			mLight.push_back( light );
	}

	/////////////////////////////////////////////////////

	void GameWorld::Save( Archive& archive )
	{
		UINT size = mGameObject.size();
		archive.Write( &size );

		TRAVERSE_VECTOR( x, mGameObject )
		{
			mGameObject[ x ]->Save( archive );
		}
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
}
