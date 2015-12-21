#pragma once

#include <vector>
#include <map>

#include "Sentinel.h"
#include "Types.h"

namespace Sentinel
{
	namespace Component
	{
		class Camera;
		class Light;
	}

	class GameObject;
	class Archive;
	class Renderer;
	class Timing;
	class PhysicsSystem;
	class AudioSystem;
	class SpriteSystem;
	class FontSystem;
	class TextureManager;
	class ShaderManager;
	class MaterialManager;
	class SpriteManager;
	class MeshManager;
	class ModelManager;
	class SoundManager;
	class RenderManager;

	class SENTINEL_DLL GameWorld
	{
		friend class GameObject;
		friend class Component::Light;
		friend class Component::Camera;

	protected:

		std::vector<GameObject*> mGameObject;
		std::vector<Component::Camera*> mCamera;
		std::vector<Component::Light*> mLight;

	public:

		Renderer* mRenderer; // must be deleted outside this class

		Timing* mTiming;
		PhysicsSystem* mPhysicsSystem;
		AudioSystem* mAudioSystem;
		SpriteSystem* mSpriteSystem;
		FontSystem* mFontSystem;

		TextureManager* mTextureManager;
		ShaderManager* mShaderManager;
		MaterialManager* mMaterialManager;
		SpriteManager* mSpriteManager;
		MeshManager* mMeshManager;
		ModelManager* mModelManager;
		SoundManager* mSoundManager;

		RenderManager* mRenderManager;
		Component::Camera* mCurrentCamera;

		/////////////////////////////////////////////////////

		GameWorld();
		~GameWorld();

		// Release all GameObjects and GameComponents.
		//
		void Release();

		// All associated GameObjects and GameComponents also Startup().
		//
		void Startup();

		// All associated GameObjects and GameComponents also Update().
		//
		void UpdateController();
		void UpdatePhysics();
		void UpdateTransform();
		void UpdateComponents();
		void UpdateCamera();
		void UpdateLight();
		void UpdateDrawable();

		// All associated GameObjects and GameComponents also Shutdown().
		//
		void Shutdown();

		/////////////////////////////////////////////////////

		// Renders all cameras if set to NULL
		//
		void Present(Component::Camera* camera = NULL);

	private:

		void PresentCamera(Component::Camera* camera);

	public:

		GameObject* AddGameObject(GameObject* obj);
		GameObject* RemoveGameObject(GameObject* obj);

		GameObject* GetGameObject(UINT index);
		GameObject* FindGameObject(const std::string& name);

	private:

		GameObject* FindGameObject(const std::string& name, GameObject* parent);

	public:

		UINT NumGameObjects();

	protected:

		Component::Camera* AddCamera(Component::Camera* camera);

		Component::Light* AddLight(Component::Light* light);

	public:

		Component::Light* GetLight(UINT index);
		UINT NumLights();

		/////////////////////////////////////////////////////

		void Save(Archive& archive);
		void Load(Archive& archive);
	};
}
