#include "AudioSource.h"

namespace Sentinel
{
	AudioSource::AudioSource() :
		mPitch( 1.0f ), mGain( 1.0f ), mLoop( false ),
		mFormat( 0 ), mSampleRate( 0 ), mDataSize( 0 ), mData( NULL )
	{}

	AudioSource::~AudioSource()
	{
		if( mData )
		{
			free( mData );
			mData = NULL;
		}
	}
}
