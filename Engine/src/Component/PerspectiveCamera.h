#pragma once

#include "Component/Camera.h"
#include "Vector3.h"
#include "Vector2.h"

namespace Sentinel
{
	namespace Component
	{
		class SENTINEL_DLL PerspectiveCamera : public Camera
		{
			static SerialRegister SERIAL_REGISTER;
			static Serializable* Clone();

		private:
			float mNearZ;
			float mFarZ;
			float mFOV;
			float mAspectRatio;

			float mAngle;

		public:
			PerspectiveCamera();
			PerspectiveCamera(UINT windowWidth, UINT windowHeight);
			PerspectiveCamera(UINT windowWidth, UINT windowHeight, float nearZ, float farZ, float FOV);

			//////////////////////////////

			void Execute();

			void Set(UINT windowWidth, UINT windowHeight);
			void Set(UINT windowWidth, UINT windowHeight, float nearZ, float farZ, float FOV);

			float NearZ();
			float FarZ();
			float FOV();
			float AspectRatio();

			float Angle();

			const Vector3& LookAt();

			void GetFrustumSize(
				Vector3& nearCenter, Vector3& farCenter,
				Vector2& nearSize, Vector2& farSize);

			Ray ScreenPointToRay(UINT mouseX, UINT mouseY, UINT screenWidth = 0, UINT screenHeight = 0);

			//////////////////////////////

			void Save(Archive& archive);
			void Load(Archive& archive);

			//////////////////////////////

			GameComponent* Copy();

		protected:
			void Copy(GameComponent* component);
		};
	}
}
