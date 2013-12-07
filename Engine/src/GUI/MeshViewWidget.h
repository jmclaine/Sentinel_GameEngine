#pragma once

#include "GUI/ViewWidget.h"

namespace Sentinel 
{ 
	class Mesh;

namespace GUI
{
	class SENTINEL_DLL MeshViewWidget : public ViewWidget
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

		std::shared_ptr< Mesh > mMesh;

		//////////////////////////////////

		MeshViewWidget();
		virtual ~MeshViewWidget();

		///////////////////////////////////

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();
	};
}}
