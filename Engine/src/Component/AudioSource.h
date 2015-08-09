#pragma once

#include "GameComponent.h"

namespace Sentinel { namespace Component
{
	class AudioSource : public GameComponent
	{
		DECLARE_SERIAL();

	public:

		AudioSource();
		~AudioSource();

		void Play();
		void Pause();
		void Stop();
	};
}}
