#pragma once

#include "TransformComponent.h"
#include "Util.h"

namespace Sentinel
{
	class DrawableComponent : public GameComponent
	{
	protected:

		// Matrices to calculate the final world matrix
		// without duplicating matrices for every object.
		//
		static Matrix4f			mMatrixTranslation;
		static Matrix4f			mMatrixScale;
		static Matrix4f			mMatrixRotation;

		//////////////////////////////////////

		TransformComponent*		mTransform;

		Material				mMaterial;

		//////////////////////////////////////

		DrawableComponent();

	public:

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();
	};
}