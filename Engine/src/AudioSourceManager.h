#pragma once

#include "AssetManager.h"
#include "AudioSource.h"

namespace Sentinel
{
	class Archive;
	class AudioSystem;

	class SENTINEL_DLL AudioSourceManager : public AssetManager< AudioSource >
	{	
	public:

		AudioSourceManager();
		~AudioSourceManager();

		/////////////////////////////////

		void Save( Archive& archive );

		void Load( Archive& archive, AudioSystem* audio );
	};
}
