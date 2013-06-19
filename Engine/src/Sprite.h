// Creates a global point mesh, generates
// sprites through a geometry shader, and 
// vertex matrix.
//
#pragma once

#include "GameWorld.h"
#include "MeshBuilder.h"
#include "OrthographicCameraComponent.h"
#include "SpriteComponent.h"

namespace Sentinel
{
	class Sprite : public Singleton< Sprite >
	{
		CameraComponent*	mCamera;		// camera to use for rendering
		CameraComponent*	mPrevCamera;	// camera to reset back to during End()

		Mesh*				mMesh;			// mesh containing only points for sprite generation

	public:

		UINT				mCount;			// number of sprites to render
		void*				mVertex;		// locked vertices within mMesh after Begin()

		/////////////////////////////////

		Sprite();
		
		void Startup( Shader* shader, UINT maxSprites = 1000 );

		void Shutdown();

		// Prepare for batched Sprite rendering.
		//
		void Begin( SpriteComponent* sprite, OrthographicCameraComponent* orthoCamera = NULL );

		// Render Sprites within the batch.
		//
		void End();
	};
};
