#include "WAudioSource.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_SHARED( AudioSource );

	DEFINE_OP_SHARED( AudioSource );

	void WAudioSource::Play()
	{
		mRef->Play();
	}

	void WAudioSource::Pause()
	{
		mRef->Pause();
	}

	void WAudioSource::Stop()
	{
		mRef->Stop();
	}

	DEFINE_CLASS_REF_SHARED( AudioSource );
}}
