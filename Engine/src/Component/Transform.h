#pragma once

#include "GameComponent.h"
#include "Matrix4f.h"
#include "Vector3f.h"
#include "Quatf.h"

namespace Sentinel { namespace Component
{
	class SENTINEL_DLL Transform : public GameComponent
	{
		DECLARE_SERIAL();

	protected:

		Transform*		mParentTransform;

		Matrix4f		mMatrixWorld;

	public:

		Vector3f		mPosition;
		Quatf			mOrientation;
		Vector3f		mScale;

		///////////////////////////////////

		Transform();

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();

		///////////////////////////////////

		const Matrix4f&	GetMatrixWorld() const;
		const Matrix4f&	GetMatrixWorld( const Vector3f& offset );

		///////////////////////////////////

		GameComponent*	Copy();
	};
}}
