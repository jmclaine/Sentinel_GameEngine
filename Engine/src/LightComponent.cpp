#include "LightComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Archive.h"
#include "Texture.h"
#include "Renderer.h"

namespace Sentinel
{
	LightComponent::LightComponent() :
		mAttenuation( Vector4f( 1.0f, 0.0f, 0.0f, 25.0f )),
		mTexture( NULL ),
		mRenderTexture( NULL )
	{
		mType = GameComponent::LIGHT;
	}

	LightComponent::~LightComponent()
	{
		SAFE_DELETE( mTexture );
		SAFE_DELETE( mRenderTexture );
	}

	TransformComponent* LightComponent::GetTransform()
	{
		return mTransform;
	}

	Texture* LightComponent::GetTexture()
	{
		return mTexture;
	}

	RenderTexture* LightComponent::GetRenderTexture()
	{
		return mRenderTexture;
	}

	///////////////////////////////////////

	void LightComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "LightComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void LightComponent::Update()
	{}

	void LightComponent::Shutdown()
	{
		mTransform = NULL;
	}

	///////////////////////////////////////

	void LightComponent::Save( Archive& archive )
	{
		GameComponent::Save( archive );

		archive.Write( mColor.Ptr(), ar_sizeof( mColor ));
		archive.Write( mAttenuation.Ptr(), ar_sizeof( mAttenuation ));
	}

	void LightComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( mColor.Ptr(), ar_sizeof( mColor ));
		archive.Read( mAttenuation.Ptr(), ar_sizeof( mAttenuation ));
	}
}
