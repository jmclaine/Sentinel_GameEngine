#pragma once

#include "GameComponent.h"

namespace Sentinel
{
	namespace Component
	{
		class Physics;

		class SENTINEL_DLL Trigger : public GameComponent
		{
		private:
			static SerialRegister SERIAL_REGISTER;
			static Serializable* Clone();

			Physics* mPhysics;

		public:
			Trigger();
			~Trigger();

			void Startup();
			void Update();
			void Shutdown();

			/////////////////////////////////

			void Save(Archive& archive);
			void Load(Archive& archive);

			/////////////////////////////////

			GameComponent* Copy();

		protected:
			void Copy(GameComponent* component);
		};
	}
}
