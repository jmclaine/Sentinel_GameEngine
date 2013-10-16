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

namespace Sentinel
{
	Mesh::Mesh()
	{
		mPrimitive = TRIANGLE_LIST;

		for( UINT i = 0; i < NUM_TEXTURES; ++i )
			mTexture[ i ] = NULL;
		
		mVBO = NULL;
		mIBO = NULL;

		mMatrixWorld.Identity();
		mMatrixShadow.Identity();
		
		mTextureScale = Vector4f( 0, 0, 1, 1 );
	}

	Mesh::~Mesh()
	{
		SAFE_DELETE( mVBO );
		SAFE_DELETE( mIBO );
	}

	void Mesh::Draw( Renderer* renderer, GameWorld* world, UINT count )
	{
		_ASSERT( world->GetCamera() != NULL );
		
		if( count == 0 )
			return;

		if( mVBO != NULL && mIBO != NULL )
		{
			renderer->SetShader( mShader );
			renderer->SetVBO( mVBO );
			renderer->SetIBO( mIBO );

			// Set Uniforms.
			//
			UINT texCount     = 0;
			UINT lightCount   = 0;
			UINT uniformIndex = 0;
			Vector3f pos;

			const std::string& uniform = mShader->Uniform();
			
			for( UINT i = 0; i < uniform.size(); ++i )
			{
				switch( uniform[ i ] )
				{
					// Model-View-Projection Matrix.
					//
					case 'P':
						mShader->SetMatrix( uniformIndex, (world->GetCamera()->mMatrixFinal * mMatrixWorld).Ptr() );
						break;

					// World Matrix.
					//
					case 'p':
						mShader->SetMatrix( uniformIndex, mMatrixWorld.Ptr() );
						break;

					// Textures.
					//
					case 'X':
						_ASSERT( mTexture[ texCount ] );	// no texture loaded

						mShader->SetTexture( uniformIndex, mTexture[ texCount ].get() );
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
						pos = world->GetCamera()->GetTransform()->mPosition;
						
						mShader->SetFloat3( uniformIndex, pos.Ptr() );
						break;

					// Light Position.
					//
					case 'L':
						mShader->SetFloat3( uniformIndex, world->GetLight( lightCount )->GetTransform()->mPosition.Ptr() );
						++uniformIndex;

						mShader->SetFloat3( uniformIndex, world->GetLight( lightCount )->mColor.Ptr() );
						++uniformIndex;

						mShader->SetFloat4( uniformIndex, world->GetLight( lightCount )->mAttenuation.Ptr() );
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
						mShader->SetMatrix( uniformIndex, (mMatrixShadow * mMatrixWorld).Ptr() );
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
						//mShader->SetFloat( uniformIndex, world->mTiming->DeltaTime() );
						break;

					// Screen-Space Ambient Occlusion.
					//
					case 'A':
						{
						const WindowInfo* info = renderer->GetWindow();

						Vector2f pixelSize( 1.0f / info->Width(), 1.0f / info->Height() );

						mShader->SetMatrix( uniformIndex, (world->GetCamera()->mMatrixProjection.Inverse()).Ptr() );
						++uniformIndex;

						mShader->SetMatrix( uniformIndex, world->GetCamera()->mMatrixView.Ptr() );
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

			renderer->SetRenderType( mPrimitive );
			renderer->DrawIndexed( (count == UINT_MAX) ? mIBO->Count() : count, 0, 0 );
		}
	}
}
