#include "GameWorld.h"
#include "GameObject.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/Light.h"
#include "Archive.h"
#include "Timing.h"

#include "PhysicsSystem.h"

#include "TextureManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"

#include "SpriteSystem.h"
#include "SpriteManager.h"
#include "MeshManager.h"
#include "ModelManager.h"

#include "FontSystem.h"
#include "FontManager.h"

#include "RenderManager.h"
#include "BlendStateManager.h"

#include "AudioSystem.h"
#include "SoundManager.h"

namespace Sentinel
{
	GameWorld::GameWorld() :
		mRenderer(nullptr),
		mCurrentCamera(nullptr)
	{ }

	GameWorld::~GameWorld()
	{
		Release();
	}

	void GameWorld::Release()
	{
		for (auto gameObject : mGameObject)
			delete gameObject;

		mGameObject.clear();
		mLight.clear();
	}

	void GameWorld::Startup()
	{
		for (auto gameObject : mGameObject)
			gameObject->Startup();
	}

	void GameWorld::UpdateController()
	{
		for (auto gameObject : mGameObject)
			gameObject->UpdateController();
	}

	void GameWorld::UpdatePhysics()
	{
		mPhysicsSystem.get()->Update(mTiming.get()->DeltaTime());

		for (auto gameObject : mGameObject)
			gameObject->UpdatePhysics();
	}

	void GameWorld::UpdateTransform()
	{
		for (auto gameObject : mGameObject)
			gameObject->UpdateTransform();
	}

	void GameWorld::UpdateComponents()
	{
		for (auto gameObject : mGameObject)
			gameObject->UpdateComponents();
	}

	void GameWorld::UpdateCamera()
	{
		for (auto gameObject : mGameObject)
			gameObject->UpdateCamera();
	}

	void GameWorld::UpdateLight()
	{
		for (auto light : mLight)
			light->Update();
	}

	void GameWorld::Shutdown()
	{
		for (auto gameObject : mGameObject)
			gameObject->Shutdown();
	}

	/////////////////////////////////////////////////////

	void GameWorld::Present(Component::Camera* camera)
	{
		if (!camera)
		{
			for (auto worldCamera : mCamera)
				PresentCamera(worldCamera);
		}
		else PresentCamera(camera);
	}

	void GameWorld::PresentCamera(Component::Camera* camera)
	{
		mCurrentCamera = camera;

		for (auto light : mLight)
			light->Draw(camera);

		camera->Apply(mRenderer);
		mRenderManager->Present(camera);
	}

	/////////////////////////////////////////////////////

	void GameWorld::AddGameObject(GameObject* obj)
	{
		_ASSERT(obj);

		for (auto gameObject : mGameObject)
		{
			if (gameObject == obj)
				return;
		}

		GameObject* parent = obj->GetParent();

		if (parent)
			parent->RemoveChild(obj);

		obj->SetWorld(this);

		mGameObject.push_back(obj);
	}

	void GameWorld::RemoveGameObject(GameObject* obj)
	{
		auto it = std::remove(mGameObject.begin(), mGameObject.end(), obj);

		if (it != mGameObject.end())
			(*it)->SetWorld(nullptr);

		mGameObject.erase(it, mGameObject.end());

		for (auto gameObject : mGameObject)
		{
			if ((*it) == obj)
			{
				mGameObject.erase(it);

				obj->SetWorld(nullptr);

				return;
			}
		}
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
		return nullptr;
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
		return nullptr;
	}

	UINT GameWorld::NumGameObjects()
	{
		return mGameObject.size();
	}

	/////////////////////////////////////////////////////

	void GameWorld::AddCamera(Component::Camera* camera)
	{
		bool isDup = false;

		for (auto item : mCamera)
		{
			if (item == camera)
			{
				isDup = true;
				break;
			}
		}

		if (!isDup)
			mCamera.push_back(camera);
	}

	void GameWorld::AddLight(Component::Light* light)
	{
		bool isDup = false;

		for (auto item : mLight)
		{
			if (item == light)
			{
				isDup = true;
				break;
			}
		}

		if (!isDup)
			mLight.push_back(light);
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

		for (auto gameObject : mGameObject)
			gameObject->Save(archive);
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
