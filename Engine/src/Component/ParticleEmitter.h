#pragma once

#include "Component/Drawable.h"

namespace Sentinel
{
	class ParticleSystem;

namespace Component
{
	class SENTINEL_DLL ParticleEmitter : public Drawable
	{
		DECLARE_SERIAL();

	protected:

		ParticleSystem*		mParticleSystem;

		/////////////////////////////////

		ParticleEmitter();

	public:

		ParticleEmitter( ParticleSystem* particle );
		~ParticleEmitter();

		void	Set( ParticleSystem* particle );

		/////////////////////////////////

		void	Startup();

		void	Update();

		void	Shutdown();

		/////////////////////////////////

		void	CalculateBounds();

		bool	CheckVisible( Camera* camera );

		void	Draw();

		///////////////////////////////////

		GameComponent* Copy();
	};
}}
