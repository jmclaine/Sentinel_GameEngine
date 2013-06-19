#pragma once

#include "Util.h"
#include "TransformComponent.h"

namespace Sentinel
{
	class LightComponent : public GameComponent
	{
	public:

		TransformComponent* mTransform;		// light must possess a position / direction

		ColorRGBA	mColor;
		vec4f		mAttenuation;			// radius = w

		//////////////////////////////

		LightComponent();

		void Startup();

		void Update();

		void Shutdown();
	};
}
