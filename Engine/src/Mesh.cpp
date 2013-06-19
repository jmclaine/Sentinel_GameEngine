#include "Timing.h"
#include "Mesh.h"
#include "GameWorld.h"

namespace Sentinel
{
	Mesh::Mesh()
	{
		mPrimitive   = TRIANGLE_LIST;

		for( UINT i = 0; i < NUM_TEXTURES; ++i )
		{
			mTexture[ i ] = Renderer::BASE_TEXTURE;
		}

		mVBO = NULL;
		mIBO = NULL;

		mNumVertices = 0;
		mNumIndices  = 0;

		mMatrixWorld.Identity();
		mMatrixShadow.Identity();
		
		mTextureScale = vec4f( 0, 0, 1, 1 );
	}

	Mesh::~Mesh()
	{
		SAFE_RELEASE_DELETE( mVBO );
		mNumVertices = 0;
		
		SAFE_RELEASE_DELETE( mIBO );
		mNumIndices = 0;
	}

	UINT Mesh::NumVertices()
	{
		return mNumVertices;
	}

	UINT Mesh::NumIndices()
	{
		return mNumIndices;
	}

	void Mesh::Draw( UINT count )
	{
		_ASSERT( GameWorld::Inst()->mCamera != NULL );

		if( count == 0 )
			return;

		if( mVBO != NULL && mIBO != NULL )
		{
			Renderer::Inst()->SetShader( mShader );
			Renderer::Inst()->SetVBO( mVBO );
			Renderer::Inst()->SetIBO( mIBO );

			// Set Uniforms.
			//
			UINT texCount     = 0;
			UINT lightCount   = 0;
			UINT uniformIndex = 0;
			vec3f pos;

			std::string uniform = mShader->UniformDecl();
			
			for( UINT i = 0; i < uniform.size(); ++i )
			{
				switch( uniform[ i ] )
				{
					// Model-View-Projection Matrix.
					//
					case 'P':
						mShader->SetMatrix( uniformIndex, (GameWorld::Inst()->mCamera->mMatrixFinal * mMatrixWorld).m );
						break;

					// World Matrix.
					//
					case 'p':
						mShader->SetMatrix( uniformIndex, mMatrixWorld.m );
						break;

					// Textures.
					//
					case 'X':
						mShader->SetTexture( uniformIndex, mTexture[ texCount ] );
						++texCount;
						break;

					// Texture Scale.
					//
					case 'x':
						mShader->SetFloat4( uniformIndex, mTextureScale.Ptr() );
						break;

					// Camera Position.
					//
					case 'V':
						pos = GameWorld::Inst()->mCamera->mTransform->mPosition;
						
						mShader->SetFloat3( uniformIndex, pos.Ptr() );
						break;

					// Light Position.
					//
					case 'L':
						pos = GameWorld::Inst()->mLight[ lightCount ]->mTransform->mPosition;
						
						mShader->SetFloat3( uniformIndex, pos.Ptr() );
						++uniformIndex;

						mShader->SetFloat3( uniformIndex, GameWorld::Inst()->mLight[ lightCount ]->mColor.Ptr() );
						++uniformIndex;

						mShader->SetFloat4( uniformIndex, GameWorld::Inst()->mLight[ lightCount ]->mAttenuation.Ptr() );
						++lightCount;

						break;

					// Material.
					//
					case 'M':
						mShader->SetFloat4( uniformIndex, mMaterial.mAmbient.Ptr() );
						++uniformIndex;

						mShader->SetFloat4( uniformIndex, mMaterial.mDiffuse.Ptr() );
						++uniformIndex;

						mShader->SetFloat4( uniformIndex, mMaterial.mSpecular.Ptr() );
						++uniformIndex;

						mShader->SetFloat(  uniformIndex, mMaterial.mSpecularComponent );
						break;

					// Shadows.
					//
					case 'S':
						mShader->SetMatrix( uniformIndex, (mMatrixShadow * mMatrixWorld).m );
						++uniformIndex;

						mShader->SetFloat( uniformIndex, 0.0f );
						break;

					// Render Target Scale.
					//
					case 's':
						mShader->SetFloat4( uniformIndex, mTextureScale.Ptr() );
						break;

					// Delta Time.
					//
					case 't':
						mShader->SetFloat( uniformIndex, Timing::Inst()->DeltaTime() );
						break;

					// Screen-Space Ambient Occlusion.
					//
					case 'A':
						{
						vec2f pixelSize( 1.0f / Renderer::SCREEN_WIDTH, 1.0f / Renderer::SCREEN_HEIGHT );

						mShader->SetMatrix( uniformIndex, (GameWorld::Inst()->mCamera->mMatrixProjection.Inverse()).m );
						++uniformIndex;

						mShader->SetMatrix( uniformIndex, GameWorld::Inst()->mCamera->mMatrixView.m );
						++uniformIndex;

						mShader->SetFloat2( uniformIndex, pixelSize.Ptr() );
						}
						break;

					// Unknown.
					//
					default:
						break;
				}

				++uniformIndex;
			}

			Renderer::Inst()->SetRenderType( mPrimitive );
			Renderer::Inst()->DrawIndexed( (count == UINT_MAX) ? mNumIndices : count, 0, 0 );
		}
	}
}
