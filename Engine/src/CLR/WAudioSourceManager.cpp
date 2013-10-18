#include "WAudioSourceManager.h"
#include "WArchive.h"
#include "WAudioSource.h"
#include "WAudioSystem.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_ASSET_MANAGER( AudioSource );

	void WAudioSourceManager::Save( WArchive^ archive )
	{
		mRef->Save( *archive );
	}

	void WAudioSourceManager::Load( WArchive^ archive, WAudioSystem^ audio )
	{
		mRef->Load( *archive, audio );
	}
}}
