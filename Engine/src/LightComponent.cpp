#include "GameObject.h"
#include "LightComponent.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( LightComponent );
	DEFINE_SERIAL_CLONE( LightComponent );

	LightComponent::LightComponent()
	{
		mType = GameComponent::LIGHT;

		mTransform = NULL;

		mAttenuation = Vector4f( 1, 1, 1, 10000 );
	}

	const TransformComponent* LightComponent::GetTransform()
	{
		return mTransform;
	}

	void LightComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "LightComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void LightComponent::Update()
	{}

	void LightComponent::Shutdown()
	{}

	//////////////////////////////

	void LightComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		archive.Write( mColor.Ptr(), 4 );
		archive.Write( mAttenuation.Ptr(), 4 );
	}

	void LightComponent::Load( Archive& archive )
	{
		archive.Read( mColor.Ptr(), 4 );
		archive.Read( mAttenuation.Ptr(), 4 );
	}
}
