#include "Material.h"
#include "Renderer.h"
#include "Archive.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace Sentinel
{
	Material::Material() :
		mAmbient( 0.2f, 0.2f, 0.2f, 1.0f ),
		mDiffuse( 0.8f, 0.8f, 0.8f, 1.0f ),
		mSpecular( 0.2f, 0.2f, 0.2f, 1.0f ),
		mSpecularComponent( 8.0f ),
		mShader( NULL ),
		mBlendState( NULL ),
		mCullMode( CULL_CCW ),
		mDepthMode( DEPTH_LEQUAL )
	{}

	Material::~Material()
	{}

	void Material::Apply( Renderer* renderer )
	{
		renderer->SetCull( mCullMode );
		renderer->SetDepthStencilType( mDepthMode );
		renderer->SetShader( mShader );
		renderer->SetBlendState( mBlendState );
	}

	//////////////////////////////////////////////

	void Material::Save( Archive&			archive, 
						 Material*			material,
						 ShaderManager*		shaderManager,
						 TextureManager*	textureManager )
	{}

	Material* Material::Load( Archive&			archive,
							  ShaderManager*	shaderManager,
							  TextureManager*	textureManager )
	{
		return NULL;
	}
}
