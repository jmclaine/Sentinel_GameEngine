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
	private:

		static bool						IS_MATERIAL_LOCKED;

	public:

		ColorRGBA						mAmbient;
		ColorRGBA						mDiffuse;
		ColorRGBA						mSpecular;
		float							mSpecularComponent;

		std::shared_ptr< Shader >		mShader;
		std::shared_ptr< Texture >		mTexture[ TextureIndex::COUNT ];
		std::shared_ptr< BlendState >	mBlendState;

		CullFormat::Type				mCullMode;
		DepthFormat::Type				mDepthMode;
		WORD							mRenderQueue;

		//////////////////////////////////////////////

		Material();
		~Material();

		void				Apply( Renderer* renderer );

		void				CalculateRenderQueue();

		// Prevent other drawables from changing the material.
		//
		static void			Lock();
		static void			Unlock();

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
