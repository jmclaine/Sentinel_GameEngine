#pragma once

#include "GameComponent.h"

namespace Sentinel
{
	class AudioComponent : public GameComponent
	{
		DECLARE_SERIAL();

	public:

		AudioComponent();
		~AudioComponent();

		void Play();
		void Pause();
		void Stop();
	};
}
