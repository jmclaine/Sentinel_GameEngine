#pragma once

#include <memory>

#include "Common.h"
#include "Mesh.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL MeshComponent : public DrawableComponent
	{
	private:

		std::shared_ptr< Mesh > mMesh;

	public:

		MeshComponent( std::shared_ptr< Mesh > mesh );

		void Startup();

		void Update();

		void Shutdown();
	};
}
