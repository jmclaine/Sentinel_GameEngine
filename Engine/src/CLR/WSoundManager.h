#pragma once

#include "Property.h"
#include "WAssetManager.h"
#include "SoundManager.h"

namespace Sentinel { namespace Wrapped
{
	ref class WArchive;
	ref class WSound;
	ref class WAudioSystem;
	
	public ref class WSoundManager
	{
		DECLARE_ASSET_MANAGER( Sound );

	public:

		void	Save( WArchive^ archive );
		void	Load( WArchive^ archive, WAudioSystem^ audio );
	};
}}
