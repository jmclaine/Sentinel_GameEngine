#pragma once
/*
Attached GameObject(s) are automatically shutdown and removed during Shutdown()
*/
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include "Common.h"
#include "Singleton.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "GameObject.h"

namespace Sentinel
{
	class SENTINEL_DLL GameWorld : public SingletonSafe< GameWorld >
	{
		friend class SingletonSafe< GameWorld >;

	private:

		std::multimap< float, GameObject* >		mAlphaOrder;

		std::vector< CameraComponent* >			mCamera;
		CameraComponent*						mCurrentCamera;

		std::vector< LightComponent* >			mLight;

		std::vector< GameObject* >				mGameObject;
		
	private:

		GameWorld();
		~GameWorld();

	public:

		// Since CLR will not save the pointer that gets
		// created by Inst(), this function is required.
		//
		static GameWorld*	Create();

		void				Save( Archive& archive );
		void				Load( Archive& archive );
		
		void				Startup();

		void				Update();
		void				UpdateController();
		void				UpdatePhysics();
		void				UpdateTransform();
		void				UpdateComponents();
		void				UpdateDrawable();

		void				Shutdown();

		//////////////////////////////

		// Returns entity.
		//
		GameObject*			AddGameObject( GameObject* entity );
		GameObject*			AddGameObject( GameObject* entity, const char* name );

		// Removes and returns entity.
		//
		GameObject*			RemoveGameObject( GameObject* entity );

		GameObject*			GetGameObject( UINT index );
		UINT				NumGameObjects();

		//////////////////////////////

		void				SetCamera( CameraComponent* camera );

		CameraComponent*	GetCamera( int index = -1 );	// -1 = mCurrentCamera
		LightComponent*		GetLight( UINT index );
	};
}
