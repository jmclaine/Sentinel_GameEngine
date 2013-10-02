#pragma once

#include <memory>

#include "Mesh.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL MeshComponent : public DrawableComponent
	{
		DECLARE_SERIAL( MeshComponent );

	public:

		std::shared_ptr< Mesh > mMesh;

		Material				mMaterial;	// sets in update

		/////////////////////////////

		MeshComponent();
		MeshComponent( std::shared_ptr< Mesh > mesh );
		~MeshComponent();

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
