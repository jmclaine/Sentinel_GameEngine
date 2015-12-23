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

		virtual ~AudioSystem() {}

		//////////////////////////////////

		// Determines the type and loads automatically.
		//
		virtual Sound*	CreateSound( const char* filename ) = 0;

		// Loads from file location into a data buffer directly.
		//
		virtual Sound*	CreateSound( Archive& archive ) = 0;

		// Loads sound by file type.
		//
		virtual Sound*	CreateSoundWAV( Archive& archive ) = 0;
		virtual Sound*	CreateSoundOGG( Archive& archive ) = 0;

		virtual void			SetListenerPosition( const Vector3& position ) = 0;
		virtual void			SetListenerVelocity( const Vector3& velocity ) = 0;
		virtual void			SetListenerOrientation( const Vector3& direction, const Vector3& up = Vector3( 0, 1, 0 )) = 0;
	};

	extern SENTINEL_DLL AudioSystem* BuildAudioSystemAL();
}