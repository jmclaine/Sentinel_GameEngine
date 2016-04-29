#pragma once

#include "GameComponent.h"

namespace Sentinel
{
	class RigidBody;

	namespace Component
	{
		class Transform;

		class SENTINEL_DLL Physics final : public GameComponent
		{
			static SerialRegister SERIAL_REGISTER;
			static Serializable* Clone();

		private:
			Transform* mTransform;
			RigidBody* mRigidBody;

		public:
			Physics();
			Physics(RigidBody* body);
			~Physics();

			void Startup();
			void Update();
			void Shutdown();

			/////////////////////////////////

			void Execute();

			void SetOwner(GameObject* owner);

			RigidBody* GetRigidBody();
			void SetRigidBody(RigidBody* body);

			///////////////////////////////////

			void Save(Archive& archive);
			void Load(Archive& archive);

			/////////////////////////////////

			GameComponent* Copy();

		private:
			void Copy(GameComponent* component);
		};
	}
}
