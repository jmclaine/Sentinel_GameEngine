#pragma once

#include "Common.h"
#include "Util.h"
#include "Material.h"
#include "TransformComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL DrawableComponent : public GameComponent
	{
	protected:

		TransformComponent*		mTransform;

	public:

		Material				mMaterial;

	protected:

		DrawableComponent();

	public:

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();
	};
}
