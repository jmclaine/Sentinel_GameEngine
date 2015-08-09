#include "Component/ParticleEmitter.h"
#include "Component/Transform.h"
#include "ParticleSystem.h"
#include "Archive.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Renderer.h"
#include "Timing.h"
#include "RenderManager.h"

namespace Sentinel { namespace Component
{
	DEFINE_SERIAL_REGISTER( ParticleEmitter );
	DEFINE_SERIAL_CLONE( ParticleEmitter );

	ParticleEmitter::ParticleEmitter() :
		mParticleSystem( NULL )
	{}

	ParticleEmitter::ParticleEmitter( ParticleSystem* particle )
	{
		Set( particle );
	}

	ParticleEmitter::~ParticleEmitter()
	{
		delete mParticleSystem;
	}

	void ParticleEmitter::Set( ParticleSystem* particle )
	{
		mParticleSystem = particle;
	}

	/////////////////////////////////

	void ParticleEmitter::Startup()
	{
		Drawable::Startup();

		mParticleSystem->Startup();

		mOwner->GetWorld()->mRenderManager->Draw( this, RenderQueue::ALPHA_BLEND, mOwner->mLayer );
	}

	void ParticleEmitter::Update()
	{
		mParticleSystem->SetMatrixWorld( mTransform->GetMatrixWorld() );

		Renderer* renderer = mOwner->GetWorld()->mRenderer;

		mParticleSystem->Update( mOwner->GetWorld()->mTiming->DeltaTime() );

		Drawable::Update();
	}

	void ParticleEmitter::Shutdown()
	{
		Drawable::Shutdown();

		mParticleSystem->Shutdown();
	}

	/////////////////////////////////

	void ParticleEmitter::CalculateBounds()
	{
		mBounds.Set( mTransform->GetMatrixWorld() );
	}

	bool ParticleEmitter::CheckVisible( Camera* camera )
	{
		return true;
	}

	void ParticleEmitter::Draw()
	{
		mParticleSystem->Draw();
	}

	/////////////////////////////////

	void ParticleEmitter::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );

		Drawable::Save( archive );

		mParticleSystem->Save( archive );
	}

	void ParticleEmitter::Load( Archive& archive )
	{
		Drawable::Load( archive );

		mParticleSystem = (ParticleSystem*)SerialRegister::Load( archive );

		GameWorld* world = mOwner->GetWorld();

		mParticleSystem->Set( world->mRenderer, world );

		mParticleSystem->Load( archive );
	}

	///////////////////////////////////

	GameComponent* ParticleEmitter::Copy()
	{
		ParticleEmitter* emitter = new ParticleEmitter( mParticleSystem );

		Drawable::Copy( emitter );

		return emitter;
	}
}}
