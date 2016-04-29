#pragma once

#include "AssetManager.h"
#include "Renderer.h"

namespace Sentinel
{
	class Archive;

	class SENTINEL_DLL BlendStateManager : public AssetManager<BlendState>
	{
	public:
		BlendStateManager();
		BlendStateManager(const BlendStateManager&) = delete;
		BlendStateManager& operator = (const BlendStateManager&) = delete;

		/////////////////////////////////

		void Save(Archive& archive, Renderer* renderer);
		void Load(Archive& archive, Renderer* renderer);
	};
}
