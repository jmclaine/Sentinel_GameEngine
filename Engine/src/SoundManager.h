#pragma once

#include "AssetManager.h"
#include "Sound.h"

namespace Sentinel
{
	class Archive;
	class AudioSystem;

	class SENTINEL_DLL SoundManager : public AssetManager<Sound>
	{
	public:
		SoundManager();

		SoundManager(const SoundManager&) = delete;
		SoundManager& operator = (const SoundManager&) = delete;

		/////////////////////////////////

		void Save(Archive& archive);
		void Load(Archive& archive, AudioSystem* audio);
	};
}
