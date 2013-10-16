#pragma once
/*
The AudioSystem is based on OpenAL, but has been created
as an abstract class to remove the core OpenAL source from
the header in order for the Editor to use it.
*/
#include "Common.h"
#include "Vector3f.h"

namespace Sentinel
{
	class Archive;
	class AudioSource;

	class SENTINEL_DLL AudioSystem
	{
	public:

		virtual ~AudioSystem() {}

		/////////////////////////////////

		static AudioSystem*		Create();

		virtual void			Startup() = 0;

		virtual void			Shutdown() = 0;

		//////////////////////////////////

		virtual AudioSource*	CreateSound( const char* filename ) = 0; // determines the type and loads automatically
		virtual AudioSource*	CreateSound( Archive& archive ) = 0;	 // loads from file location into a data buffer directly
		virtual AudioSource*	CreateSoundWAV( Archive& archive ) = 0;
		virtual AudioSource*	CreateSoundOGG( Archive& archive ) = 0;

		virtual void			SetListenerPosition( const Vector3f& position ) = 0;
		virtual void			SetListenerVelocity( const Vector3f& velocity ) = 0;
		virtual void			SetListenerOrientation( const Vector3f& direction, const Vector3f& up = Vector3f( 0, 1, 0 )) = 0;
	};
}