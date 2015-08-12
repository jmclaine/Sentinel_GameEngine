#pragma once

#include "Controller.h"

namespace Sentinel {
namespace Component
{
	class SENTINEL_DLL Controller3D : public Controller
	{
	protected:

		Controller3D();

	public:

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();
	};
}}
