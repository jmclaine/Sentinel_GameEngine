#pragma once

#include "Component/Controller3D.h"

namespace Sentinel
{
	namespace Component
	{
		class SENTINEL_DLL PlayerController final : public Controller3D
		{
			static SerialRegister SERIAL_REGISTER;
			static Serializable* Clone();

		public:
			float mSpeed;
			float mAngularSpeed;

			//////////////////////////////

			PlayerController();

			void Startup();
			void Shutdown();

			//////////////////////////////

			void Execute();

			//////////////////////////////

			void Save(Archive& archive);
			void Load(Archive& archive);

			//////////////////////////////

			GameComponent* Copy();

		private:
			void Copy(GameComponent* component);
		};
	}
}
