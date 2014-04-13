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

		TransformComponent* mTransform;

	public:

		BoundingBox		mBounds;

		bool			mIsDynamic;
		bool			mIsVisible;

	protected:

		DrawableComponent();

	public:

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();

		/////////////////////////////

		virtual void	Draw() = 0;

		virtual void	CalculateBounds() = 0;
		
	protected:

		virtual DECLARE_SERIAL_SAVE();
		virtual DECLARE_SERIAL_LOAD();
	};
}
