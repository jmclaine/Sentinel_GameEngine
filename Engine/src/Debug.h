#pragma once

#include <memory>

#include "Vector3f.h"

namespace Sentinel
{
	class Renderer;
	class GameWorld;
	class Shader;
	class Mesh;
	class Material;
	
	class SENTINEL_DLL Debug
	{
	private:

		Renderer*	mRenderer;
		GameWorld*	mWorld;

		Mesh*		mMesh;

		UINT		mNumLines;
		UINT		mMaxLines;

	public:

		std::shared_ptr< Material > mMaterial;

		Debug( Renderer* renderer, GameWorld* world, std::shared_ptr< Material > material, UINT maxLines = 1000 );
		~Debug();

		Renderer*	GetRenderer();
		GameWorld*	GetWorld();

		/////////////////////////////////////

		void	Clear();

		void	DrawLine( const Vector3f& start, const Vector3f& end );

		void	Present();
	};
}
