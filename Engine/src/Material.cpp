#include "Material.h"
#include "Renderer.h"
#include "Archive.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "BlendStateManager.h"

namespace Sentinel
{
	bool Material::IS_MATERIAL_LOCKED = false;

	Material::Material() :
		mAmbient(0.2f, 0.2f, 0.2f, 1.0f),
		mDiffuse(0.8f, 0.8f, 0.8f, 1.0f),
		mSpecular(0.2f, 0.2f, 0.2f, 1.0f),
		mSpecularComponent(8.0f),
		mCullMode(CullFormat::CCW),
		mDepthMode(DepthFormat::LEQUAL)
	{ }

	Material::~Material()
	{ }

	void Material::Apply(Renderer* renderer)
	{
		if (!IS_MATERIAL_LOCKED)
		{
			renderer->SetCull(mCullMode);
			renderer->SetDepthStencilType(mDepthMode);
			renderer->SetShader(mShader.lock().get());
			renderer->SetBlendState(mBlendState.lock().get());
		}
	}

	void Material::Lock()
	{
		IS_MATERIAL_LOCKED = true;
	}

	void Material::Unlock()
	{
		IS_MATERIAL_LOCKED = false;
	}

	//////////////////////////////////////////////

	void Material::Save(
		Archive& archive,
		Material* material,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		BlendStateManager* blendManager)
	{
		archive.Write(&material->mAmbient);
		archive.Write(&material->mDiffuse);
		archive.Write(&material->mSpecular);
		archive.Write(&material->mSpecularComponent);

		std::string shader = shaderManager->Get(material->mShader);
		archive.Write(&shader);

		std::string texture;

		for (UINT x = 0; x < (UINT)TextureIndex::COUNT; ++x)
		{
			texture = textureManager->Get(material->mTexture[x]);
			archive.Write(&texture);
		}

		auto blend = blendManager->Get(material->mBlendState);
		archive.Write(&blend);

		BYTE data = static_cast<BYTE>(material->mCullMode);
		archive.Write(&data);

		data = static_cast<BYTE>(material->mDepthMode);
		archive.Write(&data);

		archive.Write(&material->mRenderQueue);
	}

	Material* Material::Load(
		Archive& archive,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		BlendStateManager* blendManager)
	{
		Material* material = new Material();

		archive.Read(&material->mAmbient);
		archive.Read(&material->mDiffuse);
		archive.Read(&material->mSpecular);
		archive.Read(&material->mSpecularComponent);

		std::string shader;
		archive.Read(&shader);
		material->mShader = shaderManager->Get(shader);

		std::string texture;
		for (UINT x = 0; x < (UINT)TextureIndex::COUNT; ++x)
		{
			archive.Read(&texture);
			material->mTexture[x] = textureManager->Get(texture);
		}

		std::string blend;
		archive.Read(&blend);
		material->mBlendState = blendManager->Get(blend);

		BYTE cull;
		archive.Read(&cull);
		material->mCullMode = (CullFormat)cull;

		BYTE depth;
		archive.Read(&depth);
		material->mDepthMode = (DepthFormat)depth;

		WORD renderQueue;
		archive.Read(&renderQueue);
		material->mRenderQueue = renderQueue;

		return material;
	}
}
