#pragma once

#include "Model.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL ModelComponent : public DrawableComponent
	{
		DECLARE_SERIAL( ModelComponent );

	protected:

		std::vector< Material >		mMaterial;

	public:

		std::shared_ptr< Model >	mModel;

		///////////////////////////////////

		ModelComponent();
		ModelComponent( std::shared_ptr< Model > model );
		~ModelComponent();

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
