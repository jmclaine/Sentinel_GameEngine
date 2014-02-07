#pragma once

#include "LightComponent.h"
#include "Matrix4f.h"

namespace Sentinel
{
	class Shader;

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

	class SENTINEL_DLL PointLightComponent : public LightComponent
	{
		DECLARE_SERIAL();

	protected:

		UINT		mResolution;

		Matrix4f	mMatrixProjection;
		Matrix4f	mMatrixView[ NUM_CAMERA_AXIS ];
		Matrix4f	mMatrixFinal[ NUM_CAMERA_AXIS ];

	public:

		std::shared_ptr< Shader >	mShader;

		///////////////////////////////////////

		PointLightComponent();
		PointLightComponent( UINT resolution );	// creates a cube texture with dimensions of resolution^3
		~PointLightComponent();

		void		Startup();

		void		Update();

		void		Shutdown();

		///////////////////////////////////////

		const Matrix4f& GetMatrixFinal( CameraAxisType axis );

		float*		PtrMatrixFinal();

	private:

		// Adds DrawableComponents is they are dynamic,
		// and within range of the light.
		//
		void		AddDynamic( GameObject* obj );
	};
}
