#include "Mesh.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix4f.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "Archive.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Timing.h"
#include "VertexLayout.h"
#include "Material.h"

namespace Sentinel
{
	Mesh::Mesh()
	{
		mPrimitive	= TRIANGLE_LIST;

		mVBO		= NULL;
		mIBO		= NULL;

		mLayout		= NULL;
		mMaterial	= NULL;

		mMatrixWorld.Identity();
		mMatrixShadow.Identity();
	}

	Mesh::~Mesh()
	{
		SAFE_DELETE( mVBO );
		SAFE_DELETE( mIBO );
	}

	void Mesh::Draw( Renderer* renderer, GameWorld* world, UINT count )
	{
		if( count == 0 )
			return;

		if( mVBO != NULL && mIBO != NULL )
		{
			_ASSERT( renderer );
			_ASSERT( mVBO );
			_ASSERT( mIBO );
			_ASSERT( mLayout );
			_ASSERT( mMaterial );
			_ASSERT( mMaterial->mShader );
			
			mMaterial->Apply( renderer );

			renderer->SetVBO( mVBO );
			renderer->SetIBO( mIBO );

			renderer->SetVertexLayout( mLayout );

			std::shared_ptr< Shader > shader = mMaterial->mShader;

			// Set Uniforms.
			//
			UINT texCount     = 0;
			UINT lightCount   = 0;
			UINT uniformIndex = 0;
			Vector3f pos;

			const std::vector< UniformType >& uniform = shader->Uniform();
			
			for( UINT i = 0; i < uniform.size(); ++i )
			{
				switch( uniform[ i ] )
				{
				case UNIFORM_WVP:
					_ASSERT( world );
					_ASSERT( world->GetCamera() != NULL );

					shader->SetMatrix( uniformIndex, (world->GetCamera()->GetMatrixFinal() * mMatrixWorld).Ptr() );
					break;

				case UNIFORM_WORLD:
					shader->SetMatrix( uniformIndex, mMatrixWorld.Ptr() );
					break;

				case UNIFORM_TEXTURE:
					_ASSERT( mMaterial.get() );
					_ASSERT( mMaterial->mTexture[ texCount ].get() );	// no texture loaded

					shader->SetTexture( uniformIndex, mMaterial->mTexture[ texCount ].get() );
					++texCount;
					break;

				case UNIFORM_CAMERA_POS:
					_ASSERT( world );
					_ASSERT( world->GetCamera() );

					pos = world->GetCamera()->GetTransform()->mPosition;
						
					shader->SetFloat3( uniformIndex, pos.Ptr() );
					break;

				case UNIFORM_LIGHT_POS:
					_ASSERT( world );
					_ASSERT( world->GetLight( lightCount ));

					shader->SetFloat3( uniformIndex, world->GetLight( lightCount )->GetTransform()->mPosition.Ptr() );
					break;

				case UNIFORM_LIGHT_COLOR:
					_ASSERT( world );
					_ASSERT( world->GetLight( lightCount ));

					shader->SetFloat3( uniformIndex, world->GetLight( lightCount )->mColor.Ptr() );
					break;

				case UNIFORM_LIGHT_ATTN:
					_ASSERT( world );
					_ASSERT( world->GetLight( lightCount ));

					shader->SetFloat4( uniformIndex, world->GetLight( lightCount )->mAttenuation.Ptr() );
					break;

				case UNIFORM_AMBIENT:
					shader->SetFloat4( uniformIndex, mMaterial->mAmbient.Ptr() );
					break;

				case UNIFORM_DIFFUSE:
					shader->SetFloat4( uniformIndex, mMaterial->mDiffuse.Ptr() );
					break;

				case UNIFORM_SPECULAR:
					shader->SetFloat4( uniformIndex, mMaterial->mSpecular.Ptr() );
					break;

				case UNIFORM_SPEC_COMP:
					shader->SetFloat(  uniformIndex, mMaterial->mSpecularComponent );
					break;

				// Shadows.
				//
				/*case 'S':
					mShader->SetMatrix( uniformIndex, (mMatrixShadow * mMatrixWorld).Ptr() );
					++uniformIndex;

					mShader->SetFloat( uniformIndex, 0.0f );
					break;*/

				case UNIFORM_DELTA_TIME:
					_ASSERT( world );
					_ASSERT( world->mTiming );

					shader->SetFloat( uniformIndex, world->mTiming->DeltaTime() );
					break;

				// Screen-Space Ambient Occlusion.
				//
					/*
				case 'A':
					{
					_ASSERT( world );
					_ASSERT( world->GetCamera() );

					const WindowInfo* info = renderer->GetWindow();

					Vector2f pixelSize( 1.0f / info->Width(), 1.0f / info->Height() );

					mShader->SetMatrix( uniformIndex, (world->GetCamera()->GetMatrixProjection().Inverse()).Ptr() );
					++uniformIndex;

					mShader->SetMatrix( uniformIndex, const_cast< Matrix4f& >(world->GetCamera()->GetMatrixView()).Ptr() );
					++uniformIndex;

					mShader->SetFloat2( uniformIndex, pixelSize.Ptr() );
					}
					break;*/

				default:
					REPORT_ERROR( "Mesh attempted to Draw with invalid Uniform!", "Mesh Draw Error" );
					break;
				}

				++uniformIndex;
			}

			renderer->SetRenderType( mPrimitive );
			renderer->DrawIndexed( (count == UINT_MAX) ? mIBO->Count() : count, 0, 0 );
		}
	}

	void Mesh::CalculateBounds()
	{
		mBounds.Set( (BYTE*)mVBO->Lock(), mVBO->Count(), mVBO->Stride() );

		mVBO->Unlock();
	}

	//////////////////////////////

	void Mesh::Save( Archive&			archive, 
					 Mesh*				mesh,
					 Renderer*			renderer, 
					 ShaderManager*		shaderManager, 
					 TextureManager*	textureManager )
	{
		/*
		BYTE primitive = mesh->mPrimitive;
		archive.Write( &primitive );

		Buffer::Save( archive, mesh->mVBO );
		Buffer::Save( archive, mesh->mIBO );

		archive.Write( &shaderManager->Get( mesh->mShader ));

		archive.Write( (BYTE*)mesh->mMaterial.Ptr(), sizeof( Material ));

		for( UINT x = 0; x < NUM_TEXTURES; ++x )
		{
			BYTE type;
			if( mesh->mTexture[ x ].get() == renderer->NULL_TEXTURE.get() )
				type = 0;
			else
			if( mesh->mTexture[ x ].get() == renderer->BASE_TEXTURE.get() )
				type = 1;
			else
				type = 2;

			archive.Write( &type );

			if( type == 2 )
				archive.Write( &textureManager->Get( mesh->mTexture[ x ] ));
		}
		*/
	}

	Mesh* Mesh::Load( Archive&			archive, 
					  Renderer*			renderer, 
					  ShaderManager*	shaderManager, 
					  TextureManager*	textureManager )
	{
		/*
		Mesh* mesh = new Mesh();

		BYTE primitive;
		archive.Read( &primitive );
		mesh->mPrimitive = (RenderType)primitive;

		mesh->mVBO = Buffer::Load( archive, renderer );
		mesh->mIBO = Buffer::Load( archive, renderer );

		std::string shader;
		archive.Read( &shader );
		mesh->mShader = shaderManager->Get( shader );

		archive.Read( (BYTE*)mesh->mMaterial.Ptr(), sizeof( Material ));
		
		std::string texture;
		for( UINT x = 0; x < NUM_TEXTURES; ++x )
		{
			BYTE type;
			archive.Read( &type );

			if( type == 0 )
				mesh->mTexture[ x ] = renderer->NULL_TEXTURE;
			else
			if( type == 1 )
				mesh->mTexture[ x ] = renderer->BASE_TEXTURE;
			else
			{
				std::string texture;
				archive.Read( &texture );

				mesh->mTexture[ x ] = textureManager->Get( texture );

				if( mesh->mTexture[ x ] == NULL )
					mesh->mTexture[ x ] = renderer->BASE_TEXTURE;
			}
		}

		return mesh;
		*/
		return NULL;
	}
}
