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

	protected:

		TransformComponent*	mParentTransform;

		Matrix4f			mMatrixWorld;

	public:

		Vector3f			mPosition;
		Quatf				mOrientation;
		Vector3f			mScale;

		///////////////////////////////////

		TransformComponent();

		virtual void		Startup();

		virtual void		Update();

		virtual void		Shutdown();

		///////////////////////////////////

		const Matrix4f&		GetMatrixWorld() const;
	};
}
