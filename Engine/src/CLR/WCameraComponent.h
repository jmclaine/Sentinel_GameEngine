#pragma once

#include "CameraComponent.h"
#include "Property.h"
#include "WTransformComponent.h"
#include "WMatrix4f.h"

using namespace Sentinel::Wrapped;

namespace Sentinel { namespace Wrapped
{
	public ref class WCameraComponent : public WGameComponent
	{
	protected:

		WCameraComponent();

	public:

		WCameraComponent( CameraComponent* camera );

		DECLARE_CAST_COMPONENT( CameraComponent );

		const WTransformComponent^	GetTransform();

		DECLARE_PROPERTY( WMatrix4f^, MatrixView );
		DECLARE_PROPERTY( WMatrix4f^, MatrixProjection );
		DECLARE_PROPERTY( WMatrix4f^, MatrixFinal );
	};
}}
