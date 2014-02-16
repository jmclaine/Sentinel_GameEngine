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

		// Write the blend state type.
		// 0 = BLEND_OFF
		// 1 = BLEND_ALPHA
		// 2 = Custom BlendState
		//
		BYTE type;
		if( material->mBlendState.get() == renderer->BLEND_OFF.get() )
			type = 0;
		else
		if( material->mBlendState.get() == renderer->BLEND_ALPHA.get() )
			type = 1;
		else
			type = 2;

		archive.Write( &type );
		if( type == 2 )
		{
			type = (BYTE)material->mBlendState->SrcBlendColor();
			archive.Write( &type );

			type = (BYTE)material->mBlendState->DstBlendColor();
			archive.Write( &type );

			type = (BYTE)material->mBlendState->SrcBlendAlpha();
			archive.Write( &type );

			type = (BYTE)material->mBlendState->DstBlendAlpha();
			archive.Write( &type );

			type = (BYTE)material->mBlendState->BlendFuncColor();
			archive.Write( &type );

			type = (BYTE)material->mBlendState->BlendFuncAlpha();
			archive.Write( &type );
		}

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

		// Read the blend state type.
		// 0 = BLEND_OFF
		// 1 = BLEND_ALPHA
		// 2 = Custom BlendState
		//
		BYTE type;
		archive.Read( &type );

		switch( type )
		{
		case 0:
			material->mBlendState = renderer->BLEND_OFF;
			break;

		case 1:
			material->mBlendState = renderer->BLEND_ALPHA;
			break;

		default:
			{
			BYTE srcColor;
			archive.Read( &srcColor );

			BYTE dstColor;
			archive.Read( &dstColor );

			BYTE srcAlpha;
			archive.Read( &srcAlpha );

			BYTE dstAlpha;
			archive.Read( &dstAlpha );
			
			BYTE funcColor;
			archive.Read( &funcColor );
			
			BYTE funcAlpha;
			archive.Read( &funcAlpha );

			material->mBlendState = SHARED( renderer->CreateBlendState( (BlendType)srcColor, (BlendType)dstColor, 
																		(BlendType)srcAlpha, (BlendType)dstAlpha, 
																		(BlendFuncType)funcColor, (BlendFuncType)funcAlpha ));
			}
			break;
		}

		BYTE cull;
		archive.Read( &cull );
		material->mCullMode = (CullType)cull;

		BYTE depth;
		archive.Read( &depth );
		material->mDepthMode = (DepthType)depth;

		return material;
	}
}
