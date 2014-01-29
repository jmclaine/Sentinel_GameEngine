#pragma once

#include <vector>
#include <memory>

#include "DrawableComponent.h"

namespace Sentinel
{
	class Material;
	class Model;

	class SENTINEL_DLL ModelComponent : public DrawableComponent
	{
		DECLARE_SERIAL();

	public:

		std::shared_ptr< Model > mModel;

		///////////////////////////////////

		ModelComponent();
		ModelComponent( std::shared_ptr< Model > model );
		
		void	Set( std::shared_ptr< Model > model );

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
