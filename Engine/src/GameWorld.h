#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Sentinel.h"

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
	class FontManager;
	class RenderManager;
	class BlendStateManager;

	class SENTINEL_DLL GameWorld final
	{
		friend class GameObject;
		friend class Component::Light;
		friend class Component::Camera;

	private:
		std::vector<GameObject*> mGameObject;
		std::vector<Component::Camera*> mCamera;
		std::vector<Component::Light*> mLight;

	public:
		Renderer* mRenderer; // must be deleted outside this class

		std::unique_ptr<Timing> mTiming;
		std::unique_ptr<PhysicsSystem> mPhysicsSystem;

		std::unique_ptr<FontSystem> mFontSystem;
		std::unique_ptr<FontManager> mFontManager;
		
		std::unique_ptr<TextureManager> mTextureManager;
		std::unique_ptr<ShaderManager> mShaderManager;
		std::unique_ptr<MaterialManager> mMaterialManager;
		std::unique_ptr<BlendStateManager> mBlendStateManager;

		std::unique_ptr<SpriteSystem> mSpriteSystem;
		std::unique_ptr<SpriteManager> mSpriteManager;
		std::unique_ptr<MeshManager> mMeshManager;
		std::unique_ptr<ModelManager> mModelManager;
		std::unique_ptr<RenderManager> mRenderManager;

		std::unique_ptr<AudioSystem> mAudioSystem;
		std::unique_ptr<SoundManager> mSoundManager;
		
		Component::Camera* mCurrentCamera;

		/////////////////////////////////////////////////////

		GameWorld();
		~GameWorld();

		void Release();

		void Startup();

		void UpdateController();
		void UpdatePhysics();
		void UpdateTransform();
		void UpdateComponents();
		void UpdateCamera();
		void UpdateLight();

		void Shutdown();

		/////////////////////////////////////////////////////

		void Present(Component::Camera* camera = nullptr);

	private:
		void PresentCamera(Component::Camera* camera);

	public:
		void AddGameObject(GameObject* obj);
		void RemoveGameObject(GameObject* obj);

		UINT NumGameObjects();
		GameObject* GetGameObject(UINT index);
		GameObject* FindGameObject(const std::string& name);

	private:
		GameObject* FindGameObject(const std::string& name, GameObject* parent);

		void AddCamera(Component::Camera* camera);
		void AddLight(Component::Light* light);

	public:
		Component::Light* GetLight(UINT index);
		UINT NumLights();

		/////////////////////////////////////////////////////

		void Save(Archive& archive);
		void Load(Archive& archive);
	};
}
