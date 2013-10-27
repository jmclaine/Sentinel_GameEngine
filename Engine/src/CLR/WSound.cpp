#include "WSound.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_SHARED( Sound );

	DEFINE_OP_SHARED( Sound );

	void WSound::Play()
	{
		mRef->Play();
	}

	void WSound::Pause()
	{
		mRef->Pause();
	}

	void WSound::Stop()
	{
		mRef->Stop();
	}

	DEFINE_CLASS_REF_SHARED( Sound );
}}
