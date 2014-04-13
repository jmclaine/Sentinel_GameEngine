#pragma once

#include "DrawableComponent.h"

namespace Sentinel
{
	class ParticleSystem;

	class SENTINEL_DLL ParticleEmitterComponent : public DrawableComponent
	{
		DECLARE_SERIAL();

	protected:

		ParticleSystem*		mParticleSystem;

		/////////////////////////////////

		ParticleEmitterComponent();

	public:

		ParticleEmitterComponent( ParticleSystem* particle );
		~ParticleEmitterComponent();

		void	Set( ParticleSystem* particle );

		/////////////////////////////////

		void	Startup();

		void	Update();

		void	Shutdown();

		/////////////////////////////////

		void	CalculateBounds();

		void	Draw();
	};
}
