#pragma once

#include <memory>
#include <vector>

#include "Renderer.h"
#include "Mesh.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class MeshComponent : public DrawableComponent
	{
		std::shared_ptr< Mesh > mMesh;

	public:

		MeshComponent( std::shared_ptr< Mesh > mesh, const Material& material = Material() );

		void Startup();

		void Update();

		void Shutdown();
	};
}
