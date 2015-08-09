#pragma once

#include <vector>
#include <memory>

#include "Component/Drawable.h"
#include "Model.h"

namespace Sentinel
{
	class Material;
	class Model;

namespace Component
{
	class SENTINEL_DLL ModelDrawable : public Drawable
	{
		DECLARE_SERIAL();

	public:

		std::shared_ptr< Model > mModel;

		///////////////////////////////////

		ModelDrawable();
		ModelDrawable( std::shared_ptr< Model > model );
		
		void	Set( std::shared_ptr< Model > model );

		void	Startup();

		void	Update();

		void	Shutdown();

		///////////////////////////////////

		void	CalculateBounds();

		bool	CheckVisible( Camera* camera );

		void	Draw();

		///////////////////////////////////

		GameComponent* Copy();
	};
}}
