#pragma once

#include "Component/Drawable.h"

namespace Sentinel
{
	class Particle;
	class ParticleEffect;
	class Mesh;

	namespace Component
	{
		class SENTINEL_DLL ParticleEmitter : public Drawable
		{
		protected:
			bool mIsActive;

			UINT mNumParticles;
			UINT mMaxParticles;

			float mSpawnTime;

			std::unique_ptr<Mesh> mMesh;

		public:
			float mMinLifetime;
			float mMaxLifetime;

			std::vector<ParticleEffect*> mEffects;

			float mSpawnRate;

		protected:
			ParticleEmitter();
			ParticleEmitter(UINT maxParticles);

		public:
			virtual ~ParticleEmitter();

			/////////////////////////////////

			virtual void SetMaxParticles(UINT maxParticles) = 0;

			/////////////////////////////////

			virtual void Startup();
			virtual void Update();
			virtual void Shutdown();

			/////////////////////////////////

			void Draw(Camera* camera) = 0;

		protected:
			virtual void Save(Archive& archive);
			virtual void Load(Archive& archive);

			/////////////////////////////////

			virtual void Copy(GameComponent* component);

			/////////////////////////////////

			virtual Particle& GetParticle(UINT index) = 0;
		};
	}
}
