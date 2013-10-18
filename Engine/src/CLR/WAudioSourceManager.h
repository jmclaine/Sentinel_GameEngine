#pragma once

#include "Property.h"
#include "WAssetManager.h"
#include "AudioSourceManager.h"

namespace Sentinel { namespace Wrapped
{
	ref class WArchive;
	ref class WAudioSource;
	ref class WAudioSystem;
	
	public ref class WAudioSourceManager
	{
		DECLARE_ASSET_MANAGER( AudioSource );

	public:

		void	Save( WArchive^ archive );
		void	Load( WArchive^ archive, WAudioSystem^ audio );
	};
}}
