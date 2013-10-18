#pragma once

#include "WAudioSystem.h"
#include "WAudioSource.h"
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

	WAudioSource^ WAudioSystem::CreateSound( System::String^ filename )
	{
		return gcnew WAudioSource( std::shared_ptr< AudioSource >( mRef->CreateSound( WString::Cast( filename ).c_str() )));
	}

	WAudioSource^ WAudioSystem::CreateSound( WArchive^ archive )
	{
		return gcnew WAudioSource( std::shared_ptr< AudioSource >( mRef->CreateSound( *archive )));
	}

	WAudioSource^ WAudioSystem::CreateSoundWAV( WArchive^ archive )
	{
		return gcnew WAudioSource( std::shared_ptr< AudioSource >( mRef->CreateSoundWAV( *archive )));
	}

	WAudioSource^ WAudioSystem::CreateSoundOGG( WArchive^ archive )
	{
		return gcnew WAudioSource( std::shared_ptr< AudioSource >( mRef->CreateSoundOGG( *archive )));
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
