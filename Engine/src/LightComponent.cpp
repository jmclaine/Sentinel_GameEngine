#include "LightComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( LightComponent );
	DEFINE_SERIAL_CLONE( LightComponent );

	LightComponent::LightComponent() :
		mTransform( NULL ),
		mAttenuation( Vector4f( 1, 1, 1, 10000 ))
	{
		mType = GameComponent::LIGHT;
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
	{
		mTransform = NULL;
	}

	//////////////////////////////

	void LightComponent::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );

		GameComponent::Save( archive );

		archive.Write( mColor.Ptr(),		ar_sizeof( mColor ));
		archive.Write( mAttenuation.Ptr(),	ar_sizeof( mAttenuation ));
	}

	void LightComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( mColor.Ptr(),			ar_sizeof( mColor ));
		archive.Read( mAttenuation.Ptr(),	ar_sizeof( mAttenuation ));
	}
}
