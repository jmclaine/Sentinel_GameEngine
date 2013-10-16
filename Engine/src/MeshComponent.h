#pragma once

#include <memory>

#include "Mesh.h"
#include "DrawableComponent.h"
#include "Material.h"

namespace Sentinel
{
	class MeshManager;

	class SENTINEL_DLL MeshComponent : public DrawableComponent
	{
		DECLARE_SERIAL();

	protected:

		MeshManager*			mMeshManager;

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
