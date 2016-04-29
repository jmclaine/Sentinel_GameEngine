#include "SoundManager.h"
#include "AudioSystem.h"
#include "Archive.h"

namespace Sentinel
{
	SoundManager::SoundManager()
	{ }

	void SoundManager::Save(Archive& archive)
	{
		UINT count = mData.size();
		archive.Write(&count);

		for (auto it : mData)
		{
			std::shared_ptr<Sound> source = it.second;

			archive.Write(&it.first);

			Sound::Save(archive, source.get());
		}
	}

	void SoundManager::Load(Archive& archive, AudioSystem* audio)
	{
		RemoveAll();

		UINT count;
		archive.Read(&count);

		for (UINT x = 0; x < count; ++x)
		{
			std::string name;
			archive.Read(&name);

			Add(name, std::shared_ptr<Sound>(Sound::Load(archive, audio)));
		}
	}
}
