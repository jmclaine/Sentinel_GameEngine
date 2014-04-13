#include "PointLightComponent.h"
#include "TransformComponent.h"
#include "Archive.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "Renderer.h"
#include "MaterialManager.h"
#include "Texture.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( PointLightComponent );
	DEFINE_SERIAL_CLONE( PointLightComponent );

	PointLightComponent::PointLightComponent() :
		mResolution( 1024 )
	{}

	PointLightComponent::PointLightComponent( UINT resolution ) :
		mResolution( resolution )
	{}

	PointLightComponent::~PointLightComponent()
	{}

	void PointLightComponent::Startup()
	{
		LightComponent::Startup();

		Renderer* renderer = mOwner->GetWorld()->mRenderer;
		
		if( !mTexture )
			mTexture = renderer->CreateTextureCube( mResolution, mResolution, ImageFormat::RG );

		if( !mRenderTexture )
			mRenderTexture = renderer->CreateRenderTexture( mTexture );
	}

	void PointLightComponent::Update()
	{
		if( mMaterial.get() )
		{
			// Create view-projection matrices for each side of the cube.
			//
			float resolution = static_cast< float >(mResolution);
			mMatrixProjection.ProjectionPerspective( resolution, resolution, 0.0f, mAttenuation.w, 90.0f );

			const Vector3f& pos = mTransform->GetMatrixWorld().Transform( Vector3f( 0, 0, 0 ));
			
			mMatrixView[ CAMERA_AXIS_POS_X ].LookAtView( pos, pos+Vector3f( 1, 0, 0 ), Vector3f( 0, -1, 0 ));
			mMatrixFinal[ CAMERA_AXIS_POS_X ] = mMatrixProjection * mMatrixView[ CAMERA_AXIS_POS_X ];

			mMatrixView[ CAMERA_AXIS_NEG_X ].LookAtView( pos, pos+Vector3f( -1, 0, 0 ), Vector3f( 0, -1, 0 ));
			mMatrixFinal[ CAMERA_AXIS_NEG_X ] = mMatrixProjection * mMatrixView[ CAMERA_AXIS_NEG_X ];

			mMatrixView[ CAMERA_AXIS_POS_Y ].LookAtView( pos, pos+Vector3f( 0, 1, 0 ), Vector3f( 0, 0, 1 ));
			mMatrixFinal[ CAMERA_AXIS_POS_Y ] = mMatrixProjection * mMatrixView[ CAMERA_AXIS_POS_Y ];

			mMatrixView[ CAMERA_AXIS_NEG_Y ].LookAtView( pos, pos+Vector3f( 0, -1, 0 ), Vector3f( 0, 0, -1 ));
			mMatrixFinal[ CAMERA_AXIS_NEG_Y ] = mMatrixProjection * mMatrixView[ CAMERA_AXIS_NEG_Y ];

			mMatrixView[ CAMERA_AXIS_POS_Z ].LookAtView( pos, pos+Vector3f( 0, 0, 1 ), Vector3f( 0, -1, 0 ));
			mMatrixFinal[ CAMERA_AXIS_POS_Z ] = mMatrixProjection * mMatrixView[ CAMERA_AXIS_POS_Z ];

			mMatrixView[ CAMERA_AXIS_NEG_Z ].LookAtView( pos, pos+Vector3f( 0, 0, -1 ), Vector3f( 0, -1, 0 ));
			mMatrixFinal[ CAMERA_AXIS_NEG_Z ] = mMatrixProjection * mMatrixView[ CAMERA_AXIS_NEG_Z ];

			// Add all dynamic objects within range of the light.
			//
			GameWorld* world = mOwner->GetWorld();
			UINT count = world->NumGameObjects();

			mDynamic.clear();

			for( UINT x = 0; x < count; ++x )
			{
				AddDynamic( world->GetGameObject( x ));
			}

			// Draw shadow verts.
			//
			Renderer* renderer = world->mRenderer;

			static float color[4] = {1, 1, 1, 1};

			renderer->SetViewport( 0, 0, mResolution, mResolution );
			renderer->SetRenderTexture( mRenderTexture );
			renderer->ClearColor( color );

			mMaterial->Apply( renderer );

			Material::Lock();
			
			count = (UINT)mDynamic.size();
			for( UINT x = 0; x < count; ++x )
			{
				mDynamic[ x ]->Draw();
			}
			
			Material::Unlock();
		}
	}

	void PointLightComponent::Shutdown()
	{
		LightComponent::Shutdown();
	}

	///////////////////////////////////////

	const Matrix4f& PointLightComponent::GetMatrixFinal( CameraAxisType axis )
	{
		return mMatrixFinal[ axis ];
	}

	float* PointLightComponent::PtrMatrixFinal()
	{
		return mMatrixFinal[ 0 ].Ptr();
	}

	// Adds DrawableComponents if they are dynamic,
	// and within range of the light.
	//
	void PointLightComponent::AddDynamic( GameObject* obj )
	{
		DrawableComponent* drawable = (DrawableComponent*)obj->FindComponent( GameComponent::DRAWABLE );

		if( drawable )
		{
			if( drawable->mIsDynamic )
			{
				if( drawable->mBounds.Intersects( BoundingSphere( mTransform->mPosition, mAttenuation.w )))
				{
					mDynamic.push_back( drawable );
				}
			}
		}

		UINT count = obj->NumChildren();
		for( UINT x = 0; x < count; ++x )
		{
			AddDynamic( obj->GetChild( x ));
		}
	}

	///////////////////////////////////////

	void PointLightComponent::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );

		LightComponent::Save( archive );

		archive.Write( &mResolution );

		archive.Write( &mOwner->GetWorld()->mMaterialManager->Get( mMaterial ));
	}

	void PointLightComponent::Load( Archive& archive )
	{
		LightComponent::Load( archive );

		archive.Read( &mResolution );

		std::string material;
		archive.Read( &material );

		mMaterial = mOwner->GetWorld()->mMaterialManager->Get( material );
	}
}
