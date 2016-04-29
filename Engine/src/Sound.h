#pragma once

#include "Vector3.h"

namespace Sentinel
{
	class Archive;
	class AudioSystem;

	class SENTINEL_DLL Sound
	{
	public:
		float mPitch;
		float mGain;

		Vector3 mPosition;
		Vector3 mVelocity;

		bool mLoop;

	protected:
		UINT mFormat;
		DWORD mSampleRate;

		UINT mBoundSize;
		UINT mDataSize;
		BYTE* mData;

		///////////////////////////

		Sound();

	public:
		virtual ~Sound();

		///////////////////////////

		virtual void Play() = 0;
		virtual void Pause() = 0;
		virtual void Stop() = 0;

		///////////////////////////

		UINT Format();
		DWORD SampleRate();
		UINT DataSize();
		BYTE* Data();

		///////////////////////////

		static void Save(Archive& archive, Sound* source);

		static Sound* Load(Archive& archive, AudioSystem* audio);

	protected:
		void Load(Archive& archive);
	};
}
