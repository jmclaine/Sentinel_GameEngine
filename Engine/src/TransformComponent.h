#pragma once

#include "MathLib.h"
#include "GameComponent.h"

namespace Sentinel
{
	class GameComponent;

	class TransformComponent : public GameComponent
	{
	public:

		vec3f mPosition;
		quatf mOrientation;
		vec3f mScale;

		//////////////////////////////

		TransformComponent();

		void Startup();

		void Update();

		void Shutdown();
	};
}