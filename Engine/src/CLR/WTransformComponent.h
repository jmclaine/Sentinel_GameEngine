#pragma once
/*
Direct access to orientation quaternion removed for ease of editor use.
Added Rotation vector for clarity.
*/
#include "TransformComponent.h"
#include "WGameComponent.h"
#include "WMatrix4f.h"
#include "WVector3f.h"
#include "WQuatf.h"

using namespace Sentinel::Math;

namespace Sentinel { namespace Components
{
	public ref class WTransformComponent : public WGameComponent
	{
	private:

		WVector3f^	mRotation;

	public:

		WTransformComponent();
		WTransformComponent( const TransformComponent* component );
		
		DECLARE_CAST_COMPONENT( TransformComponent );

		/////////////////////////////////

		virtual void Update() override;

		/////////////////////////////////

		DECLARE_PROPERTY( WVector3f^,	Position );
		DECLARE_PROPERTY( WVector3f^,	Rotation );
		DECLARE_PROPERTY( WVector3f^,	Scale );

		/////////////////////////////////

		WMatrix4f^	GetMatrixWorld();
	};
}}
