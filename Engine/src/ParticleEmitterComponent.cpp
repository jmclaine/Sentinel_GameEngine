#include "ParticleEmitterComponent.h"
#include "ParticleSystem.h"
#include "Archive.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( ParticleEmitterComponent );
	DEFINE_SERIAL_CLONE( ParticleEmitterComponent );

	ParticleEmitterComponent::ParticleEmitterComponent() :
		mParticleSystem( NULL )
	{}

	ParticleEmitterComponent::ParticleEmitterComponent( ParticleSystem* particle )
	{
		Set( particle );
	}

	void ParticleEmitterComponent::Set( ParticleSystem* particle )
	{
		mParticleSystem = particle;
	}

	/////////////////////////////////

	void ParticleEmitterComponent::Startup()
	{}

	void ParticleEmitterComponent::Update()
	{}

	void ParticleEmitterComponent::Shutdown()
	{}

	/////////////////////////////////

	void ParticleEmitterComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );
	}

	void ParticleEmitterComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );
	}
}
