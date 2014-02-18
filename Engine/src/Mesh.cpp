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
#include "PointLightComponent.h"
#include "Archive.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
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

			Shader* shader = renderer->GetShader();

			renderer->SetVBO( mVBO );
			renderer->SetIBO( mIBO );

			renderer->SetVertexLayout( mLayout.get() );

			// Set Uniforms.
			//
			UINT texCount     = 0;
			UINT lightCount   = 0;
			UINT uniformIndex = 0;
			
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

				case UNIFORM_INV_WORLD:
					shader->SetMatrix( uniformIndex, mMatrixWorld.Inverse().Ptr() );
					break;

				case UNIFORM_VIEW:
					_ASSERT( world );
					_ASSERT( world->GetCamera() );

					shader->SetFloat3( uniformIndex, const_cast< Matrix4f& >(world->GetCamera()->GetMatrixView()).Ptr() );
					break;

				case UNIFORM_INV_VIEW:
					_ASSERT( world );
					_ASSERT( world->GetCamera() );

					shader->SetFloat3( uniformIndex, world->GetCamera()->GetMatrixView().Inverse().Ptr() );
					break;

				case UNIFORM_PROJ:
					_ASSERT( world );
					_ASSERT( world->GetCamera() );

					shader->SetFloat3( uniformIndex, const_cast< Matrix4f& >(world->GetCamera()->GetMatrixProjection()).Ptr() );
					break;

				case UNIFORM_INV_PROJ:
					_ASSERT( world );
					_ASSERT( world->GetCamera() );

					shader->SetFloat3( uniformIndex, world->GetCamera()->GetMatrixProjection().Inverse().Ptr() );
					break;

				case UNIFORM_TEXTURE:
					_ASSERT( mMaterial.get() );
					_ASSERT( mMaterial->mTexture[ texCount ].get() );	// no texture loaded

					shader->SetTexture( uniformIndex, mMaterial->mTexture[ texCount ].get() );
					++texCount;
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
					shader->SetFloat(  uniformIndex, &mMaterial->mSpecularComponent );
					break;

				case UNIFORM_CAMERA_POS:
					{
					_ASSERT( world );
					_ASSERT( world->GetCamera() );

					Vector3f pos = world->GetCamera()->GetTransform()->mPosition;
						
					shader->SetFloat3( uniformIndex, pos.Ptr() );
					}
					break;

				case UNIFORM_LIGHT_POS:
					{
					_ASSERT( world );
					_ASSERT( world->GetLight( lightCount ));

					Vector3f pos = world->GetLight( lightCount )->GetTransform()->GetMatrixWorld().Transform( Vector3f( 0, 0, 0 ));

					shader->SetFloat3( uniformIndex, pos.Ptr() );
					}
					break;

				case UNIFORM_LIGHT_DIR:
					_ASSERT( world );
					_ASSERT( world->GetLight( lightCount ));

					shader->SetFloat3( uniformIndex, world->GetLight( lightCount )->GetTransform()->mOrientation.ToEuler().Ptr() );
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

				case UNIFORM_LIGHT_TEXTURE_CUBE:
					_ASSERT( world );
					_ASSERT( world->GetLight( lightCount ));

					shader->SetTextureCube( uniformIndex, static_cast< PointLightComponent* >(world->GetLight( lightCount ))->GetTexture() );
					break;

				case UNIFORM_LIGHT_MATRIX:
					_ASSERT( world );
					_ASSERT( world->GetLight( lightCount ));

					//shader->SetMatrix( uniformIndex, static_cast< DirectionalLightComponent* >(world->GetLight( lightCount ))->PtrMatrixFinal() );
					break;

				case UNIFORM_LIGHT_CUBE_MATRIX:
					_ASSERT( world );
					_ASSERT( world->GetLight( lightCount ));

					shader->SetMatrix( uniformIndex, static_cast< PointLightComponent* >(world->GetLight( lightCount ))->PtrMatrixFinal(), 6 );
					break;

				case UNIFORM_DELTA_TIME:
					{
					_ASSERT( world );
					_ASSERT( world->mTiming );

					float DT = world->mTiming->DeltaTime();
					shader->SetFloat( uniformIndex, &DT );
					}
					break;

				default:
					_ASSERT(0);	// Mesh attempted to Draw with invalid Uniform!
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
					 TextureManager*	textureManager,
					 MaterialManager*	materialManager )
	{
		BYTE primitive = mesh->mPrimitive;
		archive.Write( &primitive );

		const std::vector< AttributeType >& layout = mesh->mLayout->Layout();

		UINT count = (UINT)layout.size();
		archive.Write( &count );

		BYTE data;
		TRAVERSE_VECTOR( x, layout )
		{
			data = layout[ x ];
			archive.Write( &data );
		}

		Buffer::Save( archive, mesh->mVBO );
		Buffer::Save( archive, mesh->mIBO );

		std::string material = materialManager->Get( mesh->mMaterial );
		archive.Write( &material );
	}

	Mesh* Mesh::Load( Archive&			archive, 
					  Renderer*			renderer, 
					  ShaderManager*	shaderManager, 
					  TextureManager*	textureManager,
					  MaterialManager*	materialManager )
	{
		Mesh* mesh = new Mesh();

		BYTE primitive;
		archive.Read( &primitive );
		mesh->mPrimitive = (RenderType)primitive;

		std::vector< AttributeType > layout;

		UINT count;
		archive.Read( &count );

		BYTE data;
		for( UINT x = 0; x < count; ++x )
		{
			archive.Read( &data );
			layout.push_back( (AttributeType)data );
		}

		mesh->mLayout = SHARED( renderer->CreateVertexLayout( layout ));

		mesh->mVBO = Buffer::Load( archive, renderer );
		mesh->mIBO = Buffer::Load( archive, renderer );

		std::string material;
		archive.Read( &material );
		mesh->mMaterial = materialManager->Get( material );

		return mesh;
	}
}
