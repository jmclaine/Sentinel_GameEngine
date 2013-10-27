#pragma once

#include <memory>

#include "DrawableComponent.h"
#include "Material.h"

namespace Sentinel
{
	class Mesh;

	class SENTINEL_DLL MeshComponent : public DrawableComponent
	{
		DECLARE_SERIAL();

	public:

		std::shared_ptr< Mesh > mMesh;

		Material				mMaterial;	// assigns to Mesh in Update

		/////////////////////////////

		MeshComponent();
		MeshComponent( std::shared_ptr< Mesh > mesh );
		
		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
