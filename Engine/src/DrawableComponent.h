#pragma once

#include "Common.h"
#include "GameComponent.h"
#include "Shape.h"

namespace Sentinel
{
	class TransformComponent;

	class SENTINEL_DLL DrawableComponent : public GameComponent
	{
	protected:

		TransformComponent*		mTransform;

		BoundingBox				mBounds;

		/////////////////////////////

		DrawableComponent();

	public:

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();

		/////////////////////////////

		const BoundingBox& GetBounds();
	};
}
