#include "Sound.h"
#include "Archive.h"
#include "AudioSystem.h"

namespace Sentinel
{
	Sound::Sound() :
		mPitch( 1.0f ), mGain( 1.0f ), mLoop( false ),
		mFormat( 0 ), mSampleRate( 0 ), mDataSize( 0 ), mData( NULL )
	{}

	Sound::~Sound()
	{
		if( mData )
		{
			free( mData );
			mData = NULL;
		}
	}

	///////////////////////////

	UINT Sound::Format()
	{
		return mFormat;
	}

	DWORD Sound::SampleRate()
	{
		return mSampleRate;
	}

	UINT Sound::DataSize()
	{
		return mDataSize;
	}

	BYTE* Sound::Data()
	{
		return mData;
	}

	///////////////////////////

	void Sound::Save( Archive& archive, Sound* source )
	{
		archive.Write( &source->mPitch );
		archive.Write( &source->mGain );
		archive.Write( source->mPosition.Ptr(), ar_sizeof( source->mPosition ));
		archive.Write( source->mVelocity.Ptr(), ar_sizeof( source->mVelocity ));

		BYTE loop = (source->mLoop) ? 1 : 0;
		archive.Write( &loop );
			
		archive.Write( &source->mFormat );
		archive.Write( &source->mSampleRate );

		archive.Write( &source->mDataSize );
			
		archive.Write( source->mData, source->mDataSize );

		archive.Write( &source->mBoundSize );
	}

	Sound* Sound::Load( Archive& archive, AudioSystem* audio )
	{
		return audio->CreateSound( archive );
	}

	void Sound::Load( Archive& archive )
	{
		archive.Read( &mPitch );
		archive.Read( &mGain );
		archive.Read( mPosition.Ptr(), ar_sizeof( mPosition ));
		archive.Read( mVelocity.Ptr(), ar_sizeof( mVelocity ));

		BYTE loop;
		archive.Read( &loop );
		mLoop = (loop) ? true : false;
			
		archive.Read( &mFormat );
		archive.Read( &mSampleRate );

		archive.Read( &mDataSize );
			
		mData = (BYTE*)malloc( mDataSize );
		archive.Read( mData, mDataSize );

		archive.Read( &mBoundSize );
	}
}
