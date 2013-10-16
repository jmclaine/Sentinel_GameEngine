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

	protected:

		std::vector< Material >		mMaterial;

	public:

		std::shared_ptr< Model >	mModel;

		///////////////////////////////////

		ModelComponent();
		ModelComponent( std::shared_ptr< Model > model );
		
		void	Set( std::shared_ptr< Model > model );

		void	Startup();

		void	Update();

		void	Shutdown();

		///////////////////////////////////

		void	SetMaterial( const Material& material );
		void	SetMaterial( UINT index, const Material& material );
		void	SetMaterial( const std::vector< Material >& material );
		const std::vector< Material >& GetMaterial();
		UINT	GetMaterialSize();
	};
}
