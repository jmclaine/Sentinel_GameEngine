#include "ParticleEmitterComponent.h"
#include "TransformComponent.h"
#include "ParticleSystem.h"
#include "Archive.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Renderer.h"
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

		Renderer* renderer = mOwner->GetWorld()->mRenderer;

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
		SERIAL_REGISTER.Save( archive );

		DrawableComponent::Save( archive );

		mParticleSystem->Save( archive );
	}

	void ParticleEmitterComponent::Load( Archive& archive )
	{
		DrawableComponent::Load( archive );

		mParticleSystem = (ParticleSystem*)SerialRegister::Load( archive );

		GameWorld* world = mOwner->GetWorld();

		mParticleSystem->Set( world->mRenderer, world );

		mParticleSystem->Load( archive );
	}
}
