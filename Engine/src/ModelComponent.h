#pragma once

#include "Common.h"
#include "Model.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL ModelComponent : public DrawableComponent
	{
	protected:

		std::vector< Material >		mMaterial;

	public:

		std::shared_ptr< Model >	mModel;

		/////////////////////////////

		ModelComponent( std::shared_ptr< Model > model );

		void	Startup();

		void	Update();

		void	Shutdown();

		/////////////////////////////

		void	SetMaterial( const Material& material );
		void	SetMaterial( UINT index, const Material& material );
		void	SetMaterial( const std::vector< Material >& material );
		const std::vector< Material >& GetMaterial();
		UINT	GetMaterialSize();
	};
}
