#pragma once

#include "GameComponent.h"
#include "Matrix4f.h"
#include "Vector3f.h"
#include "Quatf.h"

namespace Sentinel
{
	class SENTINEL_DLL TransformComponent : public GameComponent
	{
		DECLARE_SERIAL();

	private:

		// Matrices to calculate the final world matrix
		// without duplicating matrices for every object.
		//
		static Matrix4f		MATRIX_TRANSLATION;
		static Matrix4f		MATRIX_ROTATION;
		static Matrix4f		MATRIX_SCALE;

		///////////////////////////////////

		TransformComponent*	mParentTransform;

		Matrix4f			mMatrixWorld;

	public:

		Vector3f			mPosition;
		Quatf				mOrientation;
		Vector3f			mScale;

		///////////////////////////////////

		TransformComponent();

		void				Startup();

		void				Update();

		void				Shutdown();

		///////////////////////////////////

		const Matrix4f&		GetMatrixWorld();
	};
}