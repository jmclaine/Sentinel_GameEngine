#include "Component/Controller3D.h"
#include "GameObject.h"
#include "Component/Physics.h"
#include "Component/Transform.h"

namespace Sentinel {
namespace Component
{
	Controller3D::Controller3D()
	{}

	void Controller3D::Startup()
	{
		Controller::Startup();
	}

	void Controller3D::Update()
	{}

	void Controller3D::Shutdown()
	{
		Controller::Shutdown();
	}
}}
