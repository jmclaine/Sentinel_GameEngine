#pragma once

#include "WAudioSystem.h"
#include "WSound.h"
#include "WArchive.h"
#include "WString.h"
#include "WVector3f.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_PTR( AudioSystem );

	WAudioSystem::WAudioSystem( AudioSystem* audio )
	{
		mRef = audio;
	}

	void WAudioSystem::Release()
	{
		delete mRef;
	}

	DEFINE_OP_PTR( AudioSystem );

	//////////////////////////////////

	WSound^ WAudioSystem::CreateSound( System::String^ filename )
	{
		return gcnew WSound( std::shared_ptr< Sound >( mRef->CreateSound( WString::Cast( filename ).c_str() )));
	}

	WSound^ WAudioSystem::CreateSound( WArchive^ archive )
	{
		return gcnew WSound( std::shared_ptr< Sound >( mRef->CreateSound( *archive )));
	}

	WSound^ WAudioSystem::CreateSoundWAV( WArchive^ archive )
	{
		return gcnew WSound( std::shared_ptr< Sound >( mRef->CreateSoundWAV( *archive )));
	}

	WSound^ WAudioSystem::CreateSoundOGG( WArchive^ archive )
	{
		return gcnew WSound( std::shared_ptr< Sound >( mRef->CreateSoundOGG( *archive )));
	}

	void WAudioSystem::SetListenerPosition( WVector3f^ position )
	{
		mRef->SetListenerPosition( position );
	}

	void WAudioSystem::SetListenerVelocity( WVector3f^ velocity )
	{
		mRef->SetListenerVelocity( velocity );
	}

	void WAudioSystem::SetListenerOrientation( WVector3f^ direction )
	{
		mRef->SetListenerOrientation( direction );
	}

	void WAudioSystem::SetListenerOrientation( WVector3f^ direction, WVector3f^ up )
	{
		mRef->SetListenerOrientation( direction, up );
	}

	//////////////////////////////////

	WAudioSystem^ WAudioSystem::BuildAL()
	{
		return gcnew WAudioSystem( BuildAudioSystemAL() );
	}
}}
