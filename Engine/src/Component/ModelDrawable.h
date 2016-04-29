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
			static SerialRegister SERIAL_REGISTER;
			static Serializable* Clone();

		public:
			std::weak_ptr<Model> mModel;

			///////////////////////////////////

			ModelDrawable();
			ModelDrawable(std::weak_ptr<Model>& model);
			ModelDrawable(std::weak_ptr<Model>&& model);

			void Startup();
			void Update();
			void Shutdown();

			///////////////////////////////////

			void CalculateBounds();
			bool CheckVisible(Camera* camera);
			void Draw(Camera* camera);

			///////////////////////////////////

			void Save(Archive& archive);
			void Load(Archive& archive);

			///////////////////////////////////

			GameComponent* Copy();

		protected:
			void Copy(GameComponent* component);
		};
	}
}
