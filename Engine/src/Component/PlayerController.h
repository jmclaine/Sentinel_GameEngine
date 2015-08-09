#pragma once

#include "Component/Controller3D.h"

namespace Sentinel { namespace Component
{
	class SENTINEL_DLL PlayerController : public Controller3D
	{
		DECLARE_SERIAL();

	public:

		float mSpeed;
		float mAngularSpeed;

		//////////////////////////////
		
		PlayerController();

		void Startup();

		void Update();

		void Shutdown();

		//////////////////////////////

		GameComponent* Copy();
	};
}}
