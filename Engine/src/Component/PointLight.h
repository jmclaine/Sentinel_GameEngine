#pragma once

#include "Component/Light.h"
#include "Matrix4f.h"
#include "Material.h"

namespace Sentinel {
namespace Component
{
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
		DECLARE_SERIAL();

	protected:

		UINT mResolution;

		Matrix4f mMatrixProjection;
		Matrix4f mMatrixView[NUM_CAMERA_AXIS];
		Matrix4f mMatrixFinal[NUM_CAMERA_AXIS];

	public:

		std::shared_ptr<Material> mMaterial;

		///////////////////////////////////////

		PointLight();
		PointLight(UINT resolution);	// creates a cube texture with dimensions of resolution^3
		~PointLight();

		void Startup();
		void Update();
		void Shutdown();

		///////////////////////////////////////

		void Present();

		const Matrix4f& GetMatrixFinal(CameraAxisType axis);

		float* PtrMatrixFinal();

	private:

		// Adds Component::Drawables if they are dynamic,
		// and within range of the light.
		//
		void AddDynamic(GameObject* obj);

		///////////////////////////////////////

		GameComponent* Copy();
	};
}}
