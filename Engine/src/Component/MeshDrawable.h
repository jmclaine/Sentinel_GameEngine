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
		static SerialRegister SERIAL_REGISTER;
		static Serializable* Clone();

	public:
		std::weak_ptr<Mesh> mMesh;

		///////////////////////////////////

		MeshDrawable();
		MeshDrawable(std::weak_ptr<Mesh>& mesh);
		MeshDrawable(std::weak_ptr<Mesh>&& mesh);

		void Startup();
		void Update();
		void Shutdown();

		///////////////////////////////////

		void CalculateBounds();
		void Draw(Camera* camera);

		///////////////////////////////////

		void Save(Archive& archive);
		void Load(Archive& archive);

		///////////////////////////////////

		GameComponent* Copy();

	protected:
		void Copy(GameComponent* component);
	};
}}
