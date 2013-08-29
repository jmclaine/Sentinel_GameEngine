#pragma once

#include "CameraComponent.h"
#include "Property.h"
#include "WTransformComponent.h"
#include "WMatrix4f.h"

using namespace Sentinel::Math;

namespace Sentinel { namespace Components
{
	public ref class WCameraComponent : public WGameComponent
	{
	protected:

		WCameraComponent();

	public:

		WCameraComponent( CameraComponent* camera );

		const WTransformComponent^	GetTransform();

		DECLARE_PROPERTY( WMatrix4f^, MatrixView );
		DECLARE_PROPERTY( WMatrix4f^, MatrixProjection );
		DECLARE_PROPERTY( WMatrix4f^, MatrixFinal );
	};
}}
