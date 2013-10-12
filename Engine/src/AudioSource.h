#pragma once

#include "Vector3f.h"
#include "Archive.h"

namespace Sentinel
{
	class SENTINEL_DLL AudioSource
	{
	public:

		float			mPitch;
		float			mGain;
		Vector3f		mPosition;
		Vector3f		mVelocity;
		bool			mLoop;

	protected:

		// For storage purposes.
		//
		UINT			mFormat;
		DWORD			mSampleRate;

		UINT			mDataSize;
		BYTE*			mData;
   
		///////////////////////////

		AudioSource();

	public:

		virtual ~AudioSource();

		virtual void	Play() = 0;

		virtual void	Pause() = 0;

		virtual void	Stop() = 0;

		///////////////////////////

		virtual void	Save( Archive& archive ) = 0;

		virtual void	Load( Archive& archive ) = 0;
	};
}
