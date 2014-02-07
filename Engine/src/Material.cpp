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
		renderer->SetShader( mShader.get() );
		renderer->SetBlendState( mBlendState.get() );
	}

	//////////////////////////////////////////////

	void Material::Save( Archive&			archive, 
						 Material*			material,
						 Renderer*			renderer,
						 ShaderManager*		shaderManager,
						 TextureManager*	textureManager )
	{
		archive.Write( material->mAmbient.Ptr(), ar_sizeof( material->mAmbient ));
		archive.Write( material->mDiffuse.Ptr(), ar_sizeof( material->mDiffuse ));
		archive.Write( material->mSpecular.Ptr(), ar_sizeof( material->mSpecular ));
		archive.Write( &material->mSpecularComponent );

		std::string shader = shaderManager->Get( material->mShader );
		archive.Write( &shader );

		std::string texture;

		for( UINT x = 0; x < NUM_TEXTURES; ++x )
		{
			texture = textureManager->Get( material->mTexture[ x ] );
			archive.Write( &texture );
		}

		// BlendState

		BYTE data = static_cast< BYTE >(material->mCullMode);
		archive.Write( &data );

		data = static_cast< BYTE >(material->mDepthMode);
		archive.Write( &data );
	}

	Material* Material::Load( Archive&			archive,
							  Renderer*			renderer,
							  ShaderManager*	shaderManager,
							  TextureManager*	textureManager )
	{
		Material* material = new Material();

		archive.Read( material->mAmbient.Ptr(), ar_sizeof( material->mAmbient ));
		archive.Read( material->mDiffuse.Ptr(), ar_sizeof( material->mDiffuse ));
		archive.Read( material->mSpecular.Ptr(), ar_sizeof( material->mSpecular ));
		archive.Read( &material->mSpecularComponent );

		std::string shader;
		archive.Read( &shader );

		material->mShader = shaderManager->Get( shader );

		std::string texture;
		for( UINT x = 0; x < NUM_TEXTURES; ++x )
		{
			archive.Read( &texture );
			material->mTexture[ x ] = textureManager->Get( texture );
		}

		// BlendState

		BYTE cull;
		archive.Read( &cull );
		material->mCullMode = (CullType)cull;

		BYTE depth;
		archive.Read( &depth );
		material->mDepthMode = (DepthType)depth;

		return material;
	}
}
