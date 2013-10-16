#pragma once
/*
Creates a global point mesh.
Generates Sprites through a geometry shader and vertex matrix.
*/
#include "SpriteComponent.h"

namespace Sentinel
{
	class Shader;
	class Mesh;
	
	class SENTINEL_DLL ParticleSystem
	{
		friend class SpriteComponent;

	private:

		Mesh*	mMesh;			// mesh containing only points for sprite generation

	public:
		
		void*	mVertex;		// locked vertices within mMesh after Begin()
		UINT	mCount;			// number of sprites to render

		/////////////////////////////////

		ParticleSystem();
		~ParticleSystem();

		/////////////////////////////////

		void	Startup( std::shared_ptr< Shader > shader, UINT maxSprites = 1000 );

		void	Shutdown();

		/////////////////////////////////

		// Prepare for batched Sprite rendering.
		//
		void	Begin( SpriteComponent* sprite );

		// Render Sprites within the batch.
		//
		void	End();
	};
}
