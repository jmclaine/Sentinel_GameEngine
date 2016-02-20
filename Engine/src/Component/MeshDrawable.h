#pragma once

#include <memory>

#include "Component/Drawable.h"
#include "Material.h"
#include "Mesh.h"

namespace Sentinel {
namespace Component
{
	class Camera;

	class SENTINEL_DLL MeshDrawable : public Drawable
	{
		DECLARE_SERIAL();

	public:

		std::shared_ptr<Mesh> mMesh;

		///////////////////////////////////

		MeshDrawable();
		MeshDrawable(std::shared_ptr<Mesh> mesh);

		void Startup();

		///////////////////////////////////

		void Execute();

		void CalculateBounds();

		bool CheckVisible(Camera* camera);

		void Draw();

		///////////////////////////////////

		GameComponent* Copy();
	};
}}
