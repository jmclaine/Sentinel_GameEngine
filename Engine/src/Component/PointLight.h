#pragma once

#include "Component/Light.h"
#include "Matrix4x4.h"
#include "Material.h"

namespace Sentinel
{
	namespace Component
	{
		class Camera;

		enum CameraAxisType
		{
			CAMERA_AXIS_POS_X,
			CAMERA_AXIS_NEG_X,
			CAMERA_AXIS_POS_Y,
			CAMERA_AXIS_NEG_Y,
			CAMERA_AXIS_POS_Z,
			CAMERA_AXIS_NEG_Z,

			NUM_CAMERA_AXIS
		};

		class SENTINEL_DLL PointLight : public Light
		{
			static SerialRegister SERIAL_REGISTER;
			static Serializable* Clone();

		protected:
			UINT mResolution;

			Matrix4x4 mMatrixProjection;
			Matrix4x4 mMatrixView[NUM_CAMERA_AXIS];
			Matrix4x4 mMatrixFinal[NUM_CAMERA_AXIS];

		public:
			std::weak_ptr<Material> mMaterial;

			///////////////////////////////////////

			PointLight();
			PointLight(UINT resolution);	// creates a cube texture with dimensions of resolution^3
			~PointLight();

			void Startup();
			void Update();
			void Shutdown();

			///////////////////////////////////////

			void Draw(Camera* camera);

			const Matrix4x4& GetMatrixFinal(CameraAxisType axis);

			float* PtrMatrixFinal();

		private:
			void AddDynamic(GameObject* obj);

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
