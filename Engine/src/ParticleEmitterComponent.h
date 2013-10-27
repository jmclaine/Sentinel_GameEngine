#pragma once

#include "DrawableComponent.h"

namespace Sentinel
{
	class ParticleSystem;

	class ParticleEmitterComponent : public DrawableComponent
	{
		DECLARE_SERIAL();

	protected:

		ParticleSystem*		mParticleSystem;

		/////////////////////////////////

		ParticleEmitterComponent();

	public:

		ParticleEmitterComponent( ParticleSystem* particle );

		void	Set( ParticleSystem* particle );

		/////////////////////////////////

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
