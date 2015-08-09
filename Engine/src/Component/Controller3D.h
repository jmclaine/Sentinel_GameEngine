#pragma once

#include "GameComponent.h"

namespace Sentinel { namespace Component
{
	class Physics;
	class Transform;

	class SENTINEL_DLL Controller3D : public GameComponent
	{
	protected:

		Physics*		mPhysics;
		Transform*		mTransform;

		//////////////////////////////

		Controller3D();

	public:

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();
	};
}}
