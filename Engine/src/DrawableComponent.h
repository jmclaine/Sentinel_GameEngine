#pragma once

#include "Common.h"
#include "GameComponent.h"

namespace Sentinel
{
	class TransformComponent;

	class SENTINEL_DLL DrawableComponent : public GameComponent
	{
	protected:

		TransformComponent*		mTransform;

		/////////////////////////////

		DrawableComponent();

	public:

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();
	};
}
