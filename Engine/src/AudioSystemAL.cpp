#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#pragma comment (lib, "OpenAL32.lib")

#include <vorbis/vorbisfile.h>

#pragma comment (lib, "libvorbis_static.lib" )

#include "Util.h"
#include "Debug.h"
#include "AudioSystem.h"
#include "Sound.h"
#include "Archive.h"
#include "zlib.h"

namespace Sentinel
{
#define CHECK_AUDIO_ERROR(msg)\
	if (alGetError() != AL_NO_ERROR)\
		AUDIO_ERROR(msg);

#define AUDIO_ERROR(msg)\
{	delete audio;\
	Debug::ShowError(STREAM(msg), STREAM("Audio Error"));\
	return nullptr; }

#define CHECK_AUDIO_SOURCE_ERROR(msg)\
	if (alGetError() != AL_NO_ERROR)\
		Debug::ShowError(STREAM(msg), STREAM("Audio Error"));

	/////////////////////////////////////////

	class SoundAL : public Sound
	{
		friend class AudioSystemAL;

	public:
		ALuint	mSource;
		ALuint	mBuffer;

		SoundAL()
		{ }

		~SoundAL()
		{
			alDeleteSources(1, &mSource);
			alDeleteBuffers(1, &mBuffer);
		}

		/////////////////////////////////

		void Play()
		{
			alSourcef(mSource, AL_PITCH, mPitch);
			alSourcef(mSource, AL_GAIN, mGain);
			alSourcefv(mSource, AL_POSITION, mPosition.Ptr());
			alSourcefv(mSource, AL_VELOCITY, mVelocity.Ptr());
			alSourcei(mSource, AL_LOOPING, (mLoop) ? AL_TRUE : AL_FALSE);

			alSourcePlay(mSource);

			CHECK_AUDIO_SOURCE_ERROR("Failed to play sound.");
		}

		void Pause()
		{
			alSourcePause(mSource);

			CHECK_AUDIO_SOURCE_ERROR("Failed to pause sound.");
		}

		void Stop()
		{
			alSourceStop(mSource);

			CHECK_AUDIO_SOURCE_ERROR("Failed to stop sound.");
		}
	};

	/////////////////////////////////////////

	class AudioSystemAL : public AudioSystem
	{
	private:
		ALCdevice*		mDevice;
		ALCcontext*		mContext;

	public:
		AudioSystemAL()
		{
			Init();
		}

		~AudioSystemAL()
		{
			alcMakeContextCurrent(nullptr);
			alcDestroyContext(mContext);
			alcCloseDevice(mDevice);
		}

		void Init()
		{
			mDevice = alcOpenDevice(nullptr);
			if (!mDevice)
			{
				Debug::ShowError(
					"Could not open audio device!", 
					"Audio Error");

				return;
			}

			mContext = alcCreateContext(mDevice, nullptr);
			if (mContext == nullptr || alcMakeContextCurrent(mContext) == ALC_FALSE)
			{
				if (mContext != nullptr)
					alcDestroyContext(mContext);

				alcCloseDevice(mDevice);

				Debug::ShowError(
					"Could not set an audio context!", 
					"Audio Error");

				return;
			}
		}

		//////////////////////////////////

		Sound* CreateSound(const char* filename)
		{
			Debug::Log(STREAM("Loading '" << filename << "'..."));

			Archive archive;
			if (!archive.Open(filename, "rb"))
			{
				Debug::ShowError(
					STREAM("Failed to load '" << filename << "'"),
					STREAM("Audio Error"));

				return nullptr;
			}

			Sound* audio = nullptr;

			// Determine what file format to load by checking the last three chars.
			//
			int len = strlen(filename);

			if (toupper(filename[len - 3]) == 'W' && toupper(filename[len - 2]) == 'A' && toupper(filename[len - 1]) == 'V')
			{
				audio = CreateSoundWAV(archive);
			}
			else if (toupper(filename[len - 3]) == 'O' && toupper(filename[len - 2]) == 'G' && toupper(filename[len - 1]) == 'G')
			{
				audio = CreateSoundOGG(archive);
			}
			else
			{
				Debug::ShowError(
					"Sound format not supported.", 
					"Audio Error");

				return nullptr;
			}

			archive.Close();

			return audio;
		}

		Sound* CreateSound(Archive& archive)
		{
			SoundAL* source = new SoundAL();

			source->Load(archive);

			// Create buffer and source.
			//
			alGenBuffers(1, &source->mBuffer);
			alGenSources(1, &source->mSource);

			// Uncompress sound.
			//
			ULONG bound = (ULONG)source->mBoundSize;
			BYTE* data = (BYTE*)malloc(source->mBoundSize);
			uncompress(data, &bound, source->mData, source->mDataSize);

			// Create sound buffer.
			//
			alBufferData(source->mBuffer, source->mFormat, data, bound, source->mSampleRate);
			alSourcei(source->mSource, AL_BUFFER, source->mBuffer);

			free(data);

			CHECK_AUDIO_SOURCE_ERROR("Error creating sound buffer.");

			return source;
		}

		Sound* CreateSoundWAV(Archive& archive)
		{
			char  type[4];
			DWORD size, chunkSize;
			short formatType, channels;
			DWORD sampleRate, avgBytesPerSec;
			short bytesPerSample, bitsPerSample;
			DWORD dataSize;

			// Check for valid WAV file.
			//
			archive.Read(type, 4);
			if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F')
			{
				Debug::ShowError(
					"Invalid format.", 
					"Audio Error");

				return nullptr;
			}

			archive.Read(&size);
			archive.Read(type, 4);
			if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E')
			{
				Debug::ShowError(
					"Invalid format.", 
					"Audio Error");

				return nullptr;
			}

			archive.Read(type, 4);
			if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ')
			{
				Debug::ShowError(
					"Invalid format.", 
					"Audio Error");

				return nullptr;
			}

			// Read WAV info.
			//
			archive.Read(&chunkSize);
			archive.Read(&formatType);
			archive.Read(&channels);
			archive.Read(&sampleRate);
			archive.Read(&avgBytesPerSec);
			archive.Read(&bytesPerSample);
			archive.Read(&bitsPerSample);

			archive.Read(type, 4);
			if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a')
			{
				Debug::ShowError(
					"Invalid format.", 
					"Audio Error");

				return nullptr;
			}

			// Create audio source.
			//
			SoundAL* audio = new SoundAL();
			ALenum		   format = 0;

			// Read sound data.
			//
			archive.Read(&dataSize);

			BYTE* data = (BYTE*)malloc(dataSize);

			archive.Read(data, dataSize);

			// Create buffer and source.
			//
			alGenBuffers(1, &audio->mBuffer);
			alGenSources(1, &audio->mSource);

			CHECK_AUDIO_ERROR("Failed to generate source.");

			// Determine audio format.
			//
			if (bitsPerSample == 8)
			{
				if (channels == 1)
					format = AL_FORMAT_MONO8;
				else
					if (channels == 2)
						format = AL_FORMAT_STEREO8;
			}
			else
				if (bitsPerSample == 16)
				{
					if (channels == 1)
						format = AL_FORMAT_MONO16;
					else
						if (channels == 2)
							format = AL_FORMAT_STEREO16;
				}

			if (!format)
				AUDIO_ERROR("Invalid sample rate.");

			audio->mFormat = format;
			audio->mSampleRate = sampleRate;

			// Create sound buffer.
			//
			alBufferData(audio->mBuffer, audio->mFormat, data, dataSize, audio->mSampleRate);
			alSourcei(audio->mSource, AL_BUFFER, audio->mBuffer);

			// Compress sound data and store it.
			//
			ULONG bound = compressBound(dataSize);
			audio->mData = (BYTE*)malloc(bound);
			compress(audio->mData, &bound, data, dataSize);

			audio->mDataSize = (UINT)bound;
			audio->mBoundSize = dataSize;

			free(data);

			CHECK_AUDIO_ERROR("Error creating sound buffer.");

			return audio;
		}

#define BUFFER_SIZE 32767

		Sound* CreateSoundOGG(Archive& archive)
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

			if( ov_open( archive.mFile, &oggFile, nullptr, 0 ) != 0 )
			{
			REPORT_ERROR( "Error decoding...", "Audio Error" );
			return nullptr;
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
			return nullptr;
			}

			buffer.insert(buffer.end(), array, array + bytes);
			}
			while (bytes > 0);

			ov_clear( &oggFile );
			*/
			return nullptr;
		}

		void SetListenerPosition(const Vector3& position)
		{
			alListenerfv(AL_POSITION, position.Ptr());
		}

		void SetListenerVelocity(const Vector3& velocity)
		{
			alListenerfv(AL_VELOCITY, velocity.Ptr());
		}

		void SetListenerOrientation(const Vector3& direction, const Vector3& up)
		{
			float orientation[] = { direction.x, direction.y, direction.z, up.x, up.y, up.z };

			alListenerfv(AL_ORIENTATION, orientation);
		}
	};

	AudioSystem* BuildAudioSystemAL()
	{
		return new AudioSystemAL();
	}
}
