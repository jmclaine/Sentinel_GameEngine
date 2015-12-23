#include "GameWorld.h"
#include "GameObject.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/Light.h"
#include "Archive.h"
#include "Timing.h"
#include "PhysicsSystem.h"
#include "AudioSystem.h"
#include "SpriteSystem.h"
#include "FontSystem.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"
#include "SpriteManager.h"
#include "MeshManager.h"
#include "ModelManager.h"
#include "RenderManager.h"

namespace Sentinel
{
	GameWorld::GameWorld() :
		mRenderer(NULL),
		mTiming(NULL),
		mPhysicsSystem(NULL),
		mAudioSystem(NULL),
		mSpriteSystem(NULL),
		mFontSystem(NULL),
		mTextureManager(NULL),
		mShaderManager(NULL),
		mMaterialManager(NULL),
		mSpriteManager(NULL),
		mMeshManager(NULL),
		mModelManager(NULL),
		mSoundManager(NULL),
		mRenderManager(NULL),
		mCurrentCamera(NULL)
	{}

	GameWorld::~GameWorld()
	{
		Release();

		SAFE_DELETE(mTiming);
		SAFE_DELETE(mPhysicsSystem);
		SAFE_DELETE(mAudioSystem);
		SAFE_DELETE(mSpriteSystem);
		SAFE_DELETE(mFontSystem);

		SAFE_DELETE(mTextureManager);
		SAFE_DELETE(mShaderManager);
		SAFE_DELETE(mMaterialManager);
		SAFE_DELETE(mSpriteManager);
		SAFE_DELETE(mMeshManager);
		SAFE_DELETE(mModelManager);
		SAFE_DELETE(mSoundManager);

		SAFE_DELETE(mRenderManager);
	}

	void GameWorld::Release()
	{
		TRAVERSE_VECTOR(x, mGameObject)
			delete mGameObject[x];

		mGameObject.clear();
		mLight.clear();
	}

	void GameWorld::Startup()
	{
		TRAVERSE_VECTOR(x, mGameObject)
			mGameObject[x]->Startup();
	}

	void GameWorld::UpdateController()
	{
		TRAVERSE_VECTOR(x, mGameObject)
			mGameObject[x]->UpdateController();
	}

	void GameWorld::UpdatePhysics()
	{
		_ASSERT(mPhysicsSystem);
		_ASSERT(mTiming);

		mPhysicsSystem->Update(mTiming->DeltaTime());

		TRAVERSE_VECTOR(x, mGameObject)
			mGameObject[x]->UpdatePhysics();
	}

	void GameWorld::UpdateTransform()
	{
		TRAVERSE_VECTOR(x, mGameObject)
			mGameObject[x]->UpdateTransform();
	}

	void GameWorld::UpdateComponents()
	{
		TRAVERSE_VECTOR(x, mGameObject)
			mGameObject[x]->UpdateComponents();
	}

	void GameWorld::UpdateCamera()
	{
		TRAVERSE_VECTOR(x, mGameObject)
			mGameObject[x]->UpdateCamera();
	}

	void GameWorld::UpdateLight()
	{
		TRAVERSE_VECTOR(x, mLight)
			mLight[x]->Update();
	}

	void GameWorld::UpdateDrawable()
	{
		// Meshes may contain alpha values.
		// Put them in order.
		//
		// Use distance from camera to center.
		// This will not be 100% accurate, but is faster than trying to be perfect.
		//
		// TODO: Raycast from center screen to AABB for more accuracy.
		//
		/*
		Vector3 camPos = mCurrentCamera->GetTransform()->mPosition;

		mAlphaOrder.clear();
		TRAVERSE_VECTOR( x, mGameObject )
		{
		TransformComponent* transform = (TransformComponent*)mGameObject[ x ]->FindComponent( GameComponent::TRANSFORM );

		if( transform )
		mAlphaOrder.insert( std::pair< float, GameObject* >( -(camPos - transform->mPosition).LengthSquared(), mGameObject[ x ] ));
		}
		//*/

		TRAVERSE_VECTOR(x, mGameObject)
			mGameObject[x]->UpdateDrawable();
	}

	void GameWorld::Shutdown()
	{
		TRAVERSE_VECTOR(x, mGameObject)
			mGameObject[x]->Shutdown();
	}

	/////////////////////////////////////////////////////

	void GameWorld::Present(Component::Camera* camera)
	{
		if (!camera)
		{
			TRAVERSE_VECTOR(x, mCamera)
			{
				PresentCamera(mCamera[x]);
			}
		}
		else PresentCamera(camera);
	}

	void GameWorld::PresentCamera(Component::Camera* camera)
	{
		mCurrentCamera = camera;

		TRAVERSE_VECTOR(x, mLight)
		{
			mLight[x]->Present();
		}

		camera->Apply(mRenderer);
		mRenderManager->Present(camera);
	}

	/////////////////////////////////////////////////////

	GameObject* GameWorld::AddGameObject(GameObject* obj)
	{
		_ASSERT(obj);

		TRAVERSE_VECTOR(x, mGameObject)
		{
			if (mGameObject[x] == obj)
				return obj;
		}

		if (obj->GetParent())
			obj->GetParent()->RemoveChild(obj);

		obj->SetWorld(this);

		mGameObject.push_back(obj);

		return obj;
	}

	GameObject* GameWorld::RemoveGameObject(GameObject* obj)
	{
		TRAVERSE_LIST(it, mGameObject)
		{
			if ((*it) == obj)
			{
				mGameObject.erase(it);

				obj->SetWorld(NULL);

				return obj;
			}
		}

		return obj;
	}

	GameObject* GameWorld::GetGameObject(UINT index)
	{
		_ASSERT(index < mGameObject.size());

		return mGameObject[index];
	}

	GameObject* GameWorld::FindGameObject(const std::string& name)
	{
		/*TRAVERSE_VECTOR( x, mGameObject )
		{
		GetGameObject( name, mGameObject[ x ] );
		}*/
		_ASSERT(0);	// unsupported
		return NULL;
	}

	GameObject* GameWorld::FindGameObject(const std::string& name, GameObject* parent)
	{
		/*GameObject* child;

		UINT count = parent->NumChildren();
		for( UINT y = 0; y < count; ++y )
		{
		child = parent->GetChild( y );

		if( child->mName.compare( name ) == 0 )
		return child;

		GetGameObject( name, child );
		}*/
		_ASSERT(0);	// unsupported
		return NULL;
	}

	UINT GameWorld::NumGameObjects()
	{
		return mGameObject.size();
	}

	/////////////////////////////////////////////////////

	Component::Camera* GameWorld::AddCamera(Component::Camera* camera)
	{
		bool isDup = false;

		TRAVERSE_VECTOR(x, mCamera)
		{
			if (mCamera[x] == camera)
			{
				isDup = true;
				break;
			}
		}

		if (!isDup)
			mCamera.push_back(camera);

		return camera;
	}

	Component::Light* GameWorld::AddLight(Component::Light* light)
	{
		bool isDup = false;

		TRAVERSE_VECTOR(x, mLight)
		{
			if (mLight[x] == light)
			{
				isDup = true;
				break;
			}
		}

		if (!isDup)
			mLight.push_back(light);

		return light;
	}

	Component::Light* GameWorld::GetLight(UINT index)
	{
		_ASSERT(index < mLight.size());

		return mLight[index];
	}

	UINT GameWorld::NumLights()
	{
		return mLight.size();
	}

	/////////////////////////////////////////////////////

	void GameWorld::Save(Archive& archive)
	{
		UINT size = mGameObject.size();
		archive.Write(&size);

		TRAVERSE_VECTOR(x, mGameObject)
		{
			mGameObject[x]->Save(archive);
		}
	}

	void GameWorld::Load(Archive& archive)
	{
		Release();

		UINT size = 0;
		archive.Read(&size);

		GameObject* obj;
		for (UINT x = 0; x < size; ++x)
		{
			obj = (GameObject*)SerialRegister::Load(archive);
			AddGameObject(obj);
			obj->Load(archive);
		}
	}
}
