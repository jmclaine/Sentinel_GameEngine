#include "Component/ParticleEmitter.h"
#include "Component/Transform.h"
#include "Particle.h"
#include "ParticleEffect.h"
#include "Archive.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Renderer.h"
#include "Timing.h"
#include "RenderManager.h"
#include "RandomValue.h"
#include "Mesh.h"

namespace Sentinel
{
	namespace Component
	{
		ParticleEmitter::ParticleEmitter() :
			mNumParticles(0),
			mMaxParticles(0),
			mSpawnTime(0),
			mSpawnRate(0.01f),
			mIsActive(false)
		{ }

		ParticleEmitter::ParticleEmitter(UINT maxParticles) :
			mNumParticles(0),
			mMaxParticles(maxParticles),
			mSpawnTime(0),
			mSpawnRate(0.01f),
			mIsActive(false)
		{ }

		ParticleEmitter::~ParticleEmitter()
		{
			for (auto effect : mEffects)
				delete effect;
		}

		/////////////////////////////////

		void ParticleEmitter::Startup()
		{
			Drawable::Startup();

			mOwner->GetWorld()->mRenderManager->Add(this, (WORD)RenderQueue::TRANSPARENCY);

			mIsActive = true;
		}

		void ParticleEmitter::Update()
		{
			Drawable::Update();

			float DT = mOwner->GetWorld()->mTiming->DeltaTime();

			if (mNumParticles < mMaxParticles)
			{
				mSpawnTime += DT;

				while (mSpawnTime >= mSpawnRate && mNumParticles < mMaxParticles)
				{
					mSpawnTime -= mSpawnRate;

					Particle& particle = GetParticle(mNumParticles);

					particle.mElapsedTime = 0;
					particle.mLifetime = RandomValue(mMinLifetime, mMaxLifetime);
					particle.mEffectIndex = 0;

					++mNumParticles;
				}

				while (mSpawnTime >= mSpawnRate)
					mSpawnTime -= mSpawnRate;
			}

			for (int x = 0; x < (int)mNumParticles; ++x)
			{
				Particle& particle = GetParticle(x);

				for (UINT y = particle.mEffectIndex; y < (UINT)mEffects.size(); ++y)
				{
					if (particle.mElapsedTime >= mEffects[y]->mStartTime)
					{
						mEffects[y]->Startup(particle);

						++particle.mEffectIndex;
					}
				}

				for (UINT y = 0; y < particle.mEffectIndex; ++y)
					mEffects[y]->Update(particle);

				particle.Update(DT);

				if (particle.mElapsedTime >= particle.mLifetime)
				{
					--mNumParticles;

					GetParticle(x) = GetParticle(mNumParticles);

					--x;
				}
			}
		}

		void ParticleEmitter::Shutdown()
		{
			Drawable::Shutdown();

			mOwner->GetWorld()->mRenderManager->Remove(this, (WORD)RenderQueue::TRANSPARENCY);

			mNumParticles = 0;

			mIsActive = false;
		}

		/////////////////////////////////

		void ParticleEmitter::Save(Archive& archive)
		{
			Drawable::Save(archive);

			archive.Write(&mIsActive);

			archive.Write(&mMaxParticles);

			archive.Write(&mSpawnTime);

			archive.Write(&mMinLifetime);
			archive.Write(&mMaxLifetime);

			UINT size = mEffects.size();
			archive.Write(&size);

			for (auto effect : mEffects)
			{
				effect->Save(archive);
			}

			archive.Write(&mSpawnRate);
		}

		void ParticleEmitter::Load(Archive& archive)
		{
			Drawable::Load(archive);

			archive.Read(&mIsActive);

			mNumParticles = 0;
			archive.Read(&mMaxParticles);

			archive.Read(&mSpawnTime);

			archive.Read(&mMinLifetime);
			archive.Read(&mMaxLifetime);

			UINT size = 0;
			archive.Read(&size);

			ParticleEffect* effect;
			for (UINT x = 0; x < size; ++x)
			{
				effect = (ParticleEffect*)SerialRegister::Load(archive);
				mEffects.push_back(effect);
				effect->Load(archive);
			}

			archive.Read(&mSpawnRate);
		}

		///////////////////////////////////

		void ParticleEmitter::Copy(GameComponent* component)
		{
			Drawable::Copy(component);
		}
	}
}
