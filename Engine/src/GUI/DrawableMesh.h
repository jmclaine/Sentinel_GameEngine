#pragma once

#include "GUI/Widget.h"

namespace Sentinel 
{ 
	class Mesh;

namespace GUI
{
	class SENTINEL_DLL DrawableMesh : public Widget
	{
		DECLARE_SERIAL();

	public:

		std::shared_ptr< Mesh > mMesh;

		//////////////////////////////////

		DrawableMesh();
		~DrawableMesh();

		void Update();
	};
}}
