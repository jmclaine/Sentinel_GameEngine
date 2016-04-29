#pragma once

#include "GameComponent.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace Sentinel
{
	namespace Component
	{
		class SENTINEL_DLL Transform final : public GameComponent
		{
			static SerialRegister SERIAL_REGISTER;
			static Serializable* Clone();

		private:
			Transform* mParentTransform;

			Matrix4x4 mMatrixWorld;

		public:
			Vector3 mPosition;
			Quaternion mOrientation;
			Vector3 mScale;

			///////////////////////////////////

			Transform();
			~Transform();

			void Startup();
			void Update();
			void Shutdown();

			///////////////////////////////////

			void Execute();

			void SetOwner(GameObject* owner);

			const Matrix4x4& GetMatrixWorld() const;
			const Matrix4x4& GetMatrixWorld(const Vector3& offset);

			///////////////////////////////////

			void Save(Archive& archive);
			void Load(Archive& archive);

			///////////////////////////////////

			GameComponent* Copy();

		private:
			void Copy(GameComponent* component);
		};
	}
}
