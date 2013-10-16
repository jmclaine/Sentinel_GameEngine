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
	class PhysicsSystem;
	class Timing;
	class TextureManager;
	class ShaderManager;
	class MeshManager;
	class ModelManager;

	class SENTINEL_DLL GameWorld
	{
	private:

		std::multimap< float, GameObject* >		mAlphaOrder;

		std::vector< CameraComponent* >			mCamera;
		CameraComponent*						mCurrentCamera;

		std::vector< LightComponent* >			mLight;

		std::vector< GameObject* >				mGameObject;

	public:

		Renderer*								mRenderer;

		PhysicsSystem*							mPhysicsSystem;

		Timing*									mTiming;

		TextureManager*							mTextureManager;
		ShaderManager*							mShaderManager;
		MeshManager*							mMeshManager;
		ModelManager*							mModelManager;

		/////////////////////////////////////////////////////

		GameWorld();
		~GameWorld();

	public:

		// Release all GameObjects and GameComponents.
		//
		void				Release();

		void				Save( Archive& archive );
		void				Load( Archive& archive );
		
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

		// All associated GameObjects and GameComponents also Shutdown().
		//
		void				Shutdown();

		/////////////////////////////////////////////////////

		// Returns entity.
		//
		GameObject*			AddGameObject( GameObject* entity );
		GameObject*			AddGameObject( GameObject* entity, const char* name );

		// Removes and returns entity.
		//
		GameObject*			RemoveGameObject( GameObject* entity );

		GameObject*			GetGameObject( UINT index );
		UINT				NumGameObjects();

		/////////////////////////////////////////////////////

		CameraComponent*	GetCamera( int index = -1 );	// -1 = mCurrentCamera
		void				SetCamera( CameraComponent* camera );
		UINT				NumCameras();

		LightComponent*		GetLight( UINT index );
		UINT				NumLights();
	};
}
