#pragma once

#include "Common.h"
#include "Util.h"
#include "TransformComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL LightComponent : public GameComponent
	{
		TransformComponent* mTransform;		// light must possess a position / direction

	public:

		ColorRGBA	mColor;
		Vector4f	mAttenuation;			// radius = w

		//////////////////////////////

		LightComponent();

		const TransformComponent* GetTransform();

		void Startup();

		void Update();

		void Shutdown();
	};
}
