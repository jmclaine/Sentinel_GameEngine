#pragma once

#include <memory>

#include "Common.h"
#include "ColorRGBA.h"
#include "RendererTypes.h"

namespace Sentinel
{
	class Shader;
	class Texture;
	class BlendState;
	class Archive;
	class ShaderManager;
	class TextureManager;
	class Renderer;

	class SENTINEL_DLL Material
	{
	public:

		ColorRGBA						mAmbient;
		ColorRGBA						mDiffuse;
		ColorRGBA						mSpecular;
		float							mSpecularComponent;

		std::shared_ptr< Shader >		mShader;
		std::shared_ptr< Texture >		mTexture[ NUM_TEXTURES ];
		std::shared_ptr< BlendState >	mBlendState;

		CullType						mCullMode;
		DepthType						mDepthMode;

		//////////////////////////////////////////////

		Material();
		~Material();

		void				Apply( Renderer* renderer );

		//////////////////////////////////////////////

		static void			Save( Archive&			archive, 
								  Material*			material,
								  Renderer*			renderer,
								  ShaderManager*	shaderManager,
								  TextureManager*	textureManager );

		static Material*	Load( Archive&			archive,
								  Renderer*			renderer,
								  ShaderManager*	shaderManager,
								  TextureManager*	textureManager );
	};
}
