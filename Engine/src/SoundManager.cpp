#include "SoundManager.h"
#include "AudioSystem.h"
#include "Archive.h"
#include "Types.h"

namespace Sentinel
{
	SoundManager::SoundManager()
	{}

	SoundManager::~SoundManager()
	{}

	void SoundManager::Save(Archive& archive)
	{
		// Store the number of sounds.
		//
		UINT count = mData.size();
		archive.Write(&count);

		TRAVERSE_LIST(it, mData)
		{
			std::shared_ptr<Sound> source = it->second;

			// Store audio info.
			//
			archive.Write(&it->first);

			Sound::Save(archive, source.get());
		}
	}

	void SoundManager::Load(Archive& archive, AudioSystem* audio)
	{
		RemoveAll();

		// Read the number of textures to load.
		//
		UINT count;
		archive.Read(&count);

		for (UINT x = 0; x < count; ++x)
		{
			// Read texture info.
			//
			std::string name;
			archive.Read(&name);

			Add(name, std::shared_ptr<Sound>(Sound::Load(archive, audio)));
		}
	}
}
