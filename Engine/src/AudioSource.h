#pragma once

#include "Vector3f.h"

namespace Sentinel
{
	class Archive;
	class AudioSystem;

	class SENTINEL_DLL AudioSource
	{
	public:

		float				mPitch;
		float				mGain;
		Vector3f			mPosition;
		Vector3f			mVelocity;
		bool				mLoop;

	protected:

		UINT				mFormat;
		DWORD				mSampleRate;

		UINT				mBoundSize;
		UINT				mDataSize;
		BYTE*				mData;
   
		///////////////////////////

		AudioSource();

	public:

		virtual ~AudioSource();

		///////////////////////////

		virtual void		Play() = 0;

		virtual void		Pause() = 0;

		virtual void		Stop() = 0;

		///////////////////////////

		UINT				Format();

		DWORD				SampleRate();

		UINT				DataSize();

		BYTE*				Data();

		///////////////////////////

		static void			Save( Archive& archive, AudioSource* source );

		static AudioSource*	Load( Archive& archive, AudioSystem* audio );

	protected:

		void				Load( Archive& archive );
	};
}
