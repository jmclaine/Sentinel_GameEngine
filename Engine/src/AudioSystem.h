#pragma once
/*
The AudioSystem is based on OpenAL, but has been created
as an abstract class to remove the core OpenAL source from
the header in order for the Editor to use it.
*/
#include "Sentinel.h"
#include "Vector3.h"

namespace Sentinel
{
	class Archive;
	class Sound;

	// Abstract class designed for OpenAL.
	//
	// Call BuildAudioSystemAL()
	// to create the AudioSystem.
	//
	class SENTINEL_DLL AudioSystem
	{
	public:
		AudioSystem() { }
		AudioSystem(const AudioSystem&) = delete;
		AudioSystem& operator = (const AudioSystem&) = delete;
		virtual ~AudioSystem() { }

		//////////////////////////////////

		virtual Sound* CreateSound(const char* filename) = 0;
		virtual Sound* CreateSound(Archive& archive) = 0;

		virtual Sound* CreateSoundWAV(Archive& archive) = 0;
		virtual Sound* CreateSoundOGG(Archive& archive) = 0;

		virtual void SetListenerPosition(const Vector3& position) = 0;
		virtual void SetListenerVelocity(const Vector3& velocity) = 0;
		virtual void SetListenerOrientation(const Vector3& direction, const Vector3& up = Vector3(0, 1, 0)) = 0;
	};

	extern SENTINEL_DLL AudioSystem* BuildAudioSystemAL();
}