#include "WSoundManager.h"
#include "WArchive.h"
#include "WSound.h"
#include "WAudioSystem.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_ASSET_MANAGER( Sound );

	void WSoundManager::Save( WArchive^ archive )
	{
		mRef->Save( *archive );
	}

	void WSoundManager::Load( WArchive^ archive, WAudioSystem^ audio )
	{
		mRef->Load( *archive, audio );
	}
}}
