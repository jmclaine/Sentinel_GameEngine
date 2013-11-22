#include "ParticleEmitterComponent.h"
#include "TransformComponent.h"
#include "ParticleSystem.h"
#include "Archive.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Timing.h"
#include "Mesh.h"

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

	ParticleEmitterComponent::~ParticleEmitterComponent()
	{
		delete mParticleSystem;
	}

	void ParticleEmitterComponent::Set( ParticleSystem* particle )
	{
		mParticleSystem = particle;
	}

	/////////////////////////////////

	void ParticleEmitterComponent::Startup()
	{
		DrawableComponent::Startup();

		mParticleSystem->Startup();
	}

	void ParticleEmitterComponent::Update()
	{
		DrawableComponent::Update();

		mParticleSystem->SetMatrixWorld( mTransform->GetMatrixWorld() );

		mParticleSystem->Update( mOwner->GetWorld()->mTiming->DeltaTime() );
	}

	void ParticleEmitterComponent::Shutdown()
	{
		DrawableComponent::Shutdown();

		mParticleSystem->Shutdown();
	}

	/////////////////////////////////

	void ParticleEmitterComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		mParticleSystem->Save( archive );
	}

	void ParticleEmitterComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		mParticleSystem = (ParticleSystem*)SerialRegister::Load( archive );

		mParticleSystem->Set( mOwner->mWorld->mRenderer, mOwner->mWorld );

		mParticleSystem->Load( archive );
	}
}
