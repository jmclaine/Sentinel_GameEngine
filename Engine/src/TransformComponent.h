#pragma once

#include "Common.h"
#include "GameComponent.h"
#include "Vector3f.h"
#include "Quatf.h"

namespace Sentinel
{
	class SENTINEL_DLL TransformComponent : public GameComponent
	{
	public:

		Vector3f	mPosition;
		Quatf		mOrientation;
		Vector3f	mScale;

		///////////////////////////////////

		TransformComponent();

		void		Startup();

		void		Update();

		void		Shutdown();
	};
}