#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#pragma comment (lib, "OpenAL32.lib")

#include <vorbis/vorbisfile.h>

#pragma comment (lib, "libvorbis_static.lib" )

#include "AudioSystem.h"
#include "Util.h"

namespace Sentinel
{

#define CHECK_AUDIO_ERROR( msg )\
	if( alGetError() != AL_NO_ERROR )\
		AUDIO_ERROR( msg );

#define AUDIO_ERROR( msg )\
{	delete audio;\
	REPORT_ERROR( msg, "Audio Error" );\
	return NULL;	}

#define CHECK_AUDIO_SOURCE_ERROR( msg )\
	if( alGetError() != AL_NO_ERROR )\
		REPORT_ERROR( msg, "Audio Error" );

	/////////////////////////////////////////

	class AudioSourceAL : public AudioSource
	{
		friend class AudioSystemAL;

	public:

		ALuint	mSource;
		ALuint	mBuffer;

		AudioSourceAL()
		{}

		~AudioSourceAL()
		{
			alDeleteSources( 1, &mSource );
			alDeleteBuffers( 1, &mBuffer );
		}

		/////////////////////////////////

		void Play()
		{
			alSourcef ( mSource, AL_PITCH,    mPitch );
			alSourcef ( mSource, AL_GAIN,     mGain );
			alSourcefv( mSource, AL_POSITION, mPosition.Ptr() );
			alSourcefv( mSource, AL_VELOCITY, mVelocity.Ptr() );
			alSourcei ( mSource, AL_LOOPING,  (mLoop) ? AL_TRUE : AL_FALSE );
    
			alSourcePlay( mSource );
		
			CHECK_AUDIO_SOURCE_ERROR( "Failed to play sound." );
		}

		void Pause()
		{
			alSourcePause( mSource );

			CHECK_AUDIO_SOURCE_ERROR( "Failed to pause sound." );
		}

		void Stop()
		{
			alSourceStop( mSource );

			CHECK_AUDIO_SOURCE_ERROR( "Failed to stop sound." );
		}

		///////////////////////////

		void Save( Archive& archive )
		{
			archive.Write( &mPitch );
			archive.Write( &mGain );
			archive.Write( mPosition.Ptr(), ar_sizeof( mPosition ));
			archive.Write( mVelocity.Ptr(), ar_sizeof( mVelocity ));

			BYTE loop = (mLoop) ? 1 : 0;
			archive.Write( &loop );
			
			archive.Write( &mFormat );
			archive.Write( &mSampleRate );

			archive.Write( &mDataSize );
			
			archive.Write( mData, mDataSize );
		}

		void Load( Archive& archive )
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
		}
	};

	/////////////////////////////////////////

	class AudioSystemAL : public AudioSystem
	{
		friend class Archive;
	private:

		ALCdevice*		mDevice;
		ALCcontext*		mContext;

	public:

		AudioSystemAL()
		{}

		~AudioSystemAL()
		{}

		//////////////////////////////////

		void Startup()
		{
			mDevice = alcOpenDevice( NULL );
			if( !mDevice )
				throw AppException( "Could not open audio device!" );

			mContext = alcCreateContext( mDevice, NULL );
			if( mContext == NULL || alcMakeContextCurrent( mContext ) == ALC_FALSE )
			{
				if( mContext != NULL )
					alcDestroyContext( mContext );

				alcCloseDevice( mDevice );

				throw AppException( "Could not set an audio context!" );
			}
		}

		void Shutdown()
		{
			alcMakeContextCurrent( NULL );
			alcDestroyContext( mContext );
			alcCloseDevice( mDevice );
		}

		AudioSource* CreateSound( const char* filename )
		{
			TRACE( "Loading '" << filename << "'..." );

			Archive archive;
			if( !archive.Open( filename, "rb" ))
			{
				REPORT_ERROR( "Failed to load '" << filename << "'", "Audio Error" );
				return NULL;
			}

			AudioSource* audio = NULL;

			// Determine what file format to load by checking the last three chars.
			//
			int len = strlen( filename );
			if( toupper(filename[ len-3 ]) == 'W' && toupper(filename[ len-2 ]) == 'A' && toupper(filename[ len-1 ]) == 'V' )
			{
				audio = CreateSoundWAV( archive );
			}
			else
			if( toupper(filename[ len-3 ]) == 'O' && toupper(filename[ len-2 ]) == 'G' && toupper(filename[ len-1 ]) == 'G' )
			{
				audio = CreateSoundOGG( archive );
			}
			else
			{
				REPORT_ERROR( "Sound format not supported.", "Audio Error" );
				return NULL;
			}

			archive.Close();

			return audio;
		}

		AudioSource* CreateSound( Archive& archive )
		{
			AudioSourceAL* audio = new AudioSourceAL();

			audio->Load( archive );

			// Create buffer and source.
			//
			alGenBuffers( 1, &audio->mBuffer );
			alGenSources( 1, &audio->mSource );

			// Create sound buffer.
			//
			alBufferData( audio->mBuffer, audio->mFormat, audio->mData, audio->mDataSize, audio->mSampleRate );
			alSourcei( audio->mSource, AL_BUFFER, audio->mBuffer );

			CHECK_AUDIO_ERROR( "Error creating sound buffer." );
			
			return audio;
		}

		AudioSource* CreateSoundWAV( Archive& archive )
		{
			char  type[4];
			DWORD size, chunkSize;
			short formatType, channels;
			DWORD sampleRate, avgBytesPerSec;
			short bytesPerSample, bitsPerSample;
			DWORD dataSize;
    
			// Check for valid WAV file.
			//
			archive.Read( type, 4 );
			if( type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F' )
			{
				REPORT_ERROR( "Invalid format.", "Audio Error" );
				return NULL;
			}

			archive.Read( &size );
			archive.Read( type, 4 );
			if( type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E' )
			{
				REPORT_ERROR( "Invalid format.", "Audio Error" );
				return NULL;
			}

			archive.Read( type, 4 );
			if( type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ' )
			{
				REPORT_ERROR( "Invalid format.", "Audio Error" );
				return NULL;
			}
			
			// Read WAV info.
			//
			archive.Read( &chunkSize );
			archive.Read( &formatType );
			archive.Read( &channels );
			archive.Read( &sampleRate );
			archive.Read( &avgBytesPerSec );
			archive.Read( &bytesPerSample );
			archive.Read( &bitsPerSample );
    
			archive.Read( type, 4 );
			if( type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a' )
			{
				REPORT_ERROR( "Invalid format.", "Audio Error" );
				return NULL;
			}
			
			// Create audio source.
			//
			AudioSourceAL* audio  = new AudioSourceAL();
			ALenum		   format = 0;

			// Read sound data.
			//
			archive.Read( &dataSize );
    
			BYTE* data = (BYTE*)malloc( dataSize );

			archive.Read( data, dataSize );

			audio->mData		= data;
			audio->mDataSize	= dataSize;
    
			// Create buffer and source.
			//
			alGenBuffers( 1, &audio->mBuffer );
			alGenSources( 1, &audio->mSource );

			CHECK_AUDIO_ERROR( "Failed to generate source." );
			
			// Determine audio format.
			//
			if( bitsPerSample == 8 )
			{
				if( channels == 1 )
					format = AL_FORMAT_MONO8;
				else
				if( channels == 2 )
					format = AL_FORMAT_STEREO8;
			}
			else
			if( bitsPerSample == 16 )
			{
				if( channels == 1 )
					format = AL_FORMAT_MONO16;
				else
				if( channels == 2 )
					format = AL_FORMAT_STEREO16;
			}
			
			if( !format )
				AUDIO_ERROR( "Invalid sample rate." );
			
			audio->mFormat     = format;
			audio->mSampleRate = sampleRate;

			// Create sound buffer.
			//
			alBufferData( audio->mBuffer, audio->mFormat, audio->mData, audio->mDataSize, audio->mSampleRate );
			alSourcei( audio->mSource, AL_BUFFER, audio->mBuffer );

			CHECK_AUDIO_ERROR( "Error creating sound buffer." );
			
			return audio;
		}

#define BUFFER_SIZE 32767

		AudioSource* CreateSoundOGG( Archive& archive )
		{
			/*
			int				endian = 0;	// 0 for Little-Endian, 1 for Big-Endian
			int				bitStream;
			long			bytes;
			char			array[ BUFFER_SIZE ];
			ALenum			format;
			ALsizei			freq;
			
			vorbis_info*	pInfo;
			OggVorbis_File	oggFile;

			if( ov_open( archive.mFile, &oggFile, NULL, 0 ) != 0 )
			{
				REPORT_ERROR( "Error decoding...", "Audio Error" );
				return NULL;
			}
			
			pInfo = ov_info( &oggFile, -1 );

			if( pInfo->channels == 1 )
				format = AL_FORMAT_MONO16;
			else
				format = AL_FORMAT_STEREO16;
			
			freq = pInfo->rate;

			do
			{
				bytes = ov_read( &oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream );

				if( bytes < 0 )
				{
					ov_clear( &oggFile );
					REPORT_ERROR( "Error reading...", "Audio Error" );
					return NULL;
				}
				
				buffer.insert(buffer.end(), array, array + bytes);
			}
			while (bytes > 0);

			ov_clear( &oggFile );
			*/
			return NULL;
		}

		void SetListenerPosition( const Vector3f& position )
		{
			alListenerfv( AL_POSITION, position.Ptr() );
		}

		void SetListenerVelocity( const Vector3f& velocity )
		{
			alListenerfv( AL_VELOCITY, velocity.Ptr() );
		}

		void SetListenerOrientation( const Vector3f& direction, const Vector3f& up )
		{
			float orientation[] = { direction.x, direction.y, direction.z, up.x, up.y, up.z };

			alListenerfv( AL_ORIENTATION, orientation );
		}
	};

	AudioSystem* AudioSystem::Create()
	{
		return new AudioSystemAL();
	}
}
