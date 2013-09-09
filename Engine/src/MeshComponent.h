#pragma once

#include <memory>

#include "Common.h"
#include "Mesh.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL MeshComponent : public DrawableComponent
	{
	public:

		std::shared_ptr< Mesh > mMesh;

		Material				mMaterial;	// sets in update

		/////////////////////////////

		MeshComponent( std::shared_ptr< Mesh > mesh );

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
