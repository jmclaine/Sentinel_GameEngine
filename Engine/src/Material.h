#pragma once

#include <memory>

#include "Sentinel.h"
#include "ColorRGBA.h"
#include "Renderer.h"
#include "DepthStencil.h"

namespace Sentinel
{
	class Shader;
	class Texture;
	class BlendState;
	class Archive;
	class ShaderManager;
	class TextureManager;
	class BlendStateManager;

	enum class TextureIndex : BYTE
	{
		DIFFUSE,
		NORMAL,
		DEPTH,

		COUNT
	};

	class SENTINEL_DLL Material
	{
	private:
		static bool IS_MATERIAL_LOCKED;

	public:
		ColorRGBA mAmbient;
		ColorRGBA mDiffuse;
		ColorRGBA mSpecular;
		float mSpecularComponent;

		std::weak_ptr<Shader> mShader;
		std::weak_ptr<Texture> mTexture[(BYTE)TextureIndex::COUNT];
		std::weak_ptr<BlendState> mBlendState;

		CullFormat mCullMode;
		DepthFormat mDepthMode;
		WORD mRenderQueue;

		//////////////////////////////////////////////

		Material();
		~Material();

		void Apply(Renderer* renderer);

		void CalculateRenderQueue();

		static void Lock();
		static void Unlock();

		//////////////////////////////////////////////

		static void Save(
			Archive& archive,
			Material* material,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			BlendStateManager* blendManager);

		static Material* Load(
			Archive& archive,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			BlendStateManager* blendManager);
	};
}
