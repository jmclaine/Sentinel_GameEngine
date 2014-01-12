#pragma once

#include <memory>

#include "Vector3f.h"

namespace Sentinel
{
	class Renderer;
	class GameWorld;
	class Shader;
	class Mesh;

	class SENTINEL_DLL Debug
	{
	private:

		Renderer*	mRenderer;
		GameWorld*	mWorld;

		std::shared_ptr< Shader > mShader;

		Mesh*		mMesh;

		UINT		mNumLines;
		UINT		mMaxLines;

	public:

		Debug( Renderer* renderer, GameWorld* world, std::shared_ptr< Shader > shader, UINT maxLines = 1000 );
		~Debug();

		Renderer*	GetRenderer();

		GameWorld*	GetWorld();

		std::shared_ptr< Shader > GetShader();

		/////////////////////////////////////

		void	Clear();

		void	DrawLine( const Vector3f& start, const Vector3f& end );

		void	Present();
	};
}
