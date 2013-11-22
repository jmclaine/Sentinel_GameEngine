#pragma once
/*
Creates a point mesh to generate quads using a geometry shader.

Use 'sprite.xsh' to create a custom shader.  Be aware that the
vertex attributes must remain the same to ensure the
SpriteSystem can modify and render the quads correctly.
Uniforms can be added freely.

Use the current GameWorld camera to render sprites.
*/
#include <memory>

#include "Vector3f.h"
#include "Matrix4f.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class Renderer;
	class GameWorld;
	class Mesh;
	class Sprite;
	class Shader;

	class SENTINEL_DLL SpriteSystem
	{
	private:

		Renderer*			mRenderer;
		
		Mesh*				mMesh;

		UINT				mNumSprites;
		UINT				mMaxSprites;
		
	public:

		struct Storage
		{
		public:

			UINT			mFrame;
			ColorRGBA		mColor;

			Matrix4f		mMatrixWorld;
		};

	private:

		Storage*			mStorage;

	public:

		std::shared_ptr< Sprite > mSprite;

		/////////////////////////////////////

		SpriteSystem( Renderer* renderer, std::shared_ptr< Shader > shader, UINT maxSprites );
		~SpriteSystem();

		void		Clear();

		void		Draw( UINT frame, const ColorRGBA& color, const Matrix4f& matWorld );

		void		Present();
	};
}
