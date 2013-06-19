#pragma once

#include "TransformComponent.h"
#include "Util.h"

namespace Sentinel
{
	class DrawableComponent : public GameComponent
	{
	protected:

		static mat4f			mMatrixTranslation;
		static mat4f			mMatrixScale;
		static mat4f			mMatrixRotation;

		TransformComponent*		mTransform;
		Material				mMaterial;

	public:

		DrawableComponent();

		void Startup();

		void Update();

		void Shutdown();
	};
}
