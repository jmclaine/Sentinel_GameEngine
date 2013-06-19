#pragma once

#include "Model.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class ModelComponent : public DrawableComponent
	{
		std::shared_ptr< Model > mModel;
		
	public:

		ModelComponent( std::shared_ptr< Model > model, const Material& material = Material() );

		void Startup();

		void Update();

		void Shutdown();
	};
}
