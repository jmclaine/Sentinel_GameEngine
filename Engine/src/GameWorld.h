#pragma once

#include <vector>
#include <map>

#include "Common.h"

namespace Sentinel
{
	class GameObject;
	class CameraComponent;
	class LightComponent;
	class Archive;
	class Renderer;
	class Timing;
	class PhysicsSystem;
	class AudioSystem;
	class SpriteSystem;
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
	private:

		std::vector< CameraComponent* >			mCamera;
		CameraComponent*						mCurrentCamera;

		std::vector< LightComponent* >			mLight;

		std::vector< GameObject* >				mGameObject;

	public:

		Renderer*								mRenderer;		// must be deleted outside this class

		Timing*									mTiming;
		PhysicsSystem*							mPhysicsSystem;
		AudioSystem*							mAudioSystem;
		SpriteSystem*							mSpriteSystem;
		
		TextureManager*							mTextureManager;
		ShaderManager*							mShaderManager;
		MaterialManager*						mMaterialManager;
		SpriteManager*							mSpriteManager;
		MeshManager*							mMeshManager;
		ModelManager*							mModelManager;
		SoundManager*							mSoundManager;

		RenderManager*							mRenderManager;

		/////////////////////////////////////////////////////

		GameWorld();
		~GameWorld();

	public:

		// Release all GameObjects and GameComponents.
		//
		void				Release();

		// All associated GameObjects and GameComponents also Startup().
		//
		void				Startup();

		// All associated GameObjects and GameComponents also Update().
		//
		void				UpdateController();
		void				UpdatePhysics();
		void				UpdateTransform();
		void				UpdateComponents();
		void				UpdateDrawable();
		void				UpdateLight();

		// All associated GameObjects and GameComponents also Shutdown().
		//
		void				Shutdown();

		/////////////////////////////////////////////////////

		void				Present();

		/////////////////////////////////////////////////////

		// Returns entity.
		//
		GameObject*			AddGameObject( GameObject* obj );
		GameObject*			AddGameObject( GameObject* obj, const char* name );

		// Removes and returns entity.
		//
		GameObject*			RemoveGameObject( GameObject* obj );

		GameObject*			GetGameObject( UINT index );
		UINT				NumGameObjects();

		/////////////////////////////////////////////////////

		CameraComponent*	GetCamera( int index = -1 );	// -1 = mCurrentCamera
		void				SetCamera( CameraComponent* camera );
		UINT				NumCameras();
		void				AddCamera( CameraComponent* camera );

		LightComponent*		GetLight( UINT index );
		UINT				NumLights();
		void				AddLight( LightComponent* light );

		/////////////////////////////////////////////////////

		void				Save( Archive& archive );
		void				Load( Archive& archive );
	};
}
