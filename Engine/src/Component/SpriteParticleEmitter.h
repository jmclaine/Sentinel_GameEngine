#pragma once

#include "Component/ParticleEmitter.h"

namespace Sentinel
{
	class Material;
	class Sprite;
	class SpriteParticle;

	namespace Component
	{
		class SENTINEL_DLL SpriteParticleEmitter : public ParticleEmitter
		{
			static SerialRegister SERIAL_REGISTER;
			static Serializable* Clone();

		public:
			std::weak_ptr<Material> mMaterial;
			std::weak_ptr<Sprite> mSprite;

		protected:
			SpriteParticle* mParticle;

		public:
			SpriteParticleEmitter();
			SpriteParticleEmitter(UINT maxParticles);
			~SpriteParticleEmitter();

			/////////////////////////////////

			void SetMaxParticles(UINT maxParticles);

			/////////////////////////////////

			void Startup();
			void Update();
			void Shutdown();

			/////////////////////////////////

			void Draw(Camera* camera);

			/////////////////////////////////

			void Save(Archive& archive);
			void Load(Archive& archive);

			///////////////////////////////////

			GameComponent* Copy();

		protected:
			void Copy(GameComponent* component);

			Particle& GetParticle(UINT index);
		};
	}
}