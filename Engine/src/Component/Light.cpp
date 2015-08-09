#include "Light.h"
#include "Component/Transform.h"
#include "GameObject.h"
#include "Archive.h"
#include "Texture.h"
#include "Renderer.h"

namespace Sentinel { namespace Component
{
	Light::Light() :
		mAttenuation( Vector4f( 1.0f, 0.0f, 0.0f, 25.0f )),
		mTexture( NULL ),
		mRenderTexture( NULL )
	{
		mType = GameComponent::LIGHT;
	}

	Light::~Light()
	{
		SAFE_DELETE( mTexture );
		SAFE_DELETE( mRenderTexture );
	}

	Transform* Light::GetTransform()
	{
		return mTransform;
	}

	Texture* Light::GetTexture()
	{
		return mTexture;
	}

	RenderTexture* Light::GetRenderTexture()
	{
		return mRenderTexture;
	}

	///////////////////////////////////////

	void Light::Startup()
	{
		mTransform = (Transform*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "Light::Startup()\n" + std::string( mOwner->mName ) + " does not contain Transform" );
	}

	void Light::Update()
	{}

	void Light::Shutdown()
	{
		mTransform = NULL;
	}

	///////////////////////////////////////

	void Light::Save( Archive& archive )
	{
		GameComponent::Save( archive );

		archive.Write( mColor.Ptr(), ar_sizeof( mColor ));
		archive.Write( mAttenuation.Ptr(), ar_sizeof( mAttenuation ));
	}

	void Light::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( mColor.Ptr(), ar_sizeof( mColor ));
		archive.Read( mAttenuation.Ptr(), ar_sizeof( mAttenuation ));
	}

	///////////////////////////////////////

	void Light::Copy( GameComponent* component )
	{
		Light* light = (Light*)component;

		GameComponent::Copy( light );

		light->mColor = mColor;
		light->mAttenuation = mAttenuation;
	}
}}
