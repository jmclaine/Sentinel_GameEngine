#pragma once

#include "GameComponent.h"
#include "ColorRGBA.h"
#include "Vector4f.h"

namespace Sentinel
{
	class TransformComponent;

	class SENTINEL_DLL LightComponent : public GameComponent
	{
		DECLARE_SERIAL();

		TransformComponent* mTransform;		// light must possess a position / direction

	public:

		ColorRGBA	mColor;
		Vector4f	mAttenuation;			// radius = w

		//////////////////////////////

		LightComponent();

		const TransformComponent* GetTransform();

		void		Startup();

		void		Update();

		void		Shutdown();
	};
}
