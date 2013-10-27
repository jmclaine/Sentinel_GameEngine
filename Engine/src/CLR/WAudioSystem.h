#pragma once

#include "Property.h"
#include "AudioSystem.h"

namespace Sentinel { namespace Wrapped
{
	ref class WSound;
	ref class WArchive;
	ref class WVector3f;

	public ref class WAudioSystem
	{
		DECLARE_REF_PTR( AudioSystem );

	public:

		WAudioSystem( AudioSystem* audio );

		void			Release();

		DECLARE_OP_PTR( AudioSystem );

		//////////////////////////////////

		// Determines the type and loads automatically.
		//
		WSound^	CreateSound( System::String^ filename );

		// Loads from file location into a data buffer directly.
		//
		WSound^	CreateSound( WArchive^ archive );

		// Loads sound by file type.
		//
		WSound^	CreateSoundWAV( WArchive^ archive );
		WSound^	CreateSoundOGG( WArchive^ archive );

		void			SetListenerPosition( WVector3f^ position );
		void			SetListenerVelocity( WVector3f^ velocity );
		void			SetListenerOrientation( WVector3f^ direction );
		void			SetListenerOrientation( WVector3f^ direction, WVector3f^ up );

		//////////////////////////////////

		static WAudioSystem^ BuildAL();
	};
}}
