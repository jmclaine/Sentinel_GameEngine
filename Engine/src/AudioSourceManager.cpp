#include "AudioSourceManager.h"
#include "AudioSystem.h"
#include "Archive.h"

namespace Sentinel
{
	AudioSourceManager::AudioSourceManager()
	{}

	AudioSourceManager::~AudioSourceManager()
	{}

	void AudioSourceManager::Save( Archive& archive )
	{
		// Store the number of sounds.
		//
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			std::shared_ptr< AudioSource > source = it->second;

			// Store audio info.
			//
			archive.Write( &it->first );

			AudioSource::Save( archive, source.get() );
		}
	}

	void AudioSourceManager::Load( Archive& archive, AudioSystem* audio )
	{
		RemoveAll();

		// Read the number of textures to load.
		//
		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			// Read texture info.
			//
			std::string name;
			archive.Read( &name );

			Add( name, std::shared_ptr< AudioSource >( AudioSource::Load( archive, audio )));
		}
	}
}
