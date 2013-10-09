#pragma once

#include "WGameComponent.h"
#include "WVector3f.h"
#include "WQuatf.h"
#include "CustomPhysicsComponent.h"

using namespace Sentinel::Math;

namespace Sentinel { namespace Components
{
	public enum class ShapeType
	{
		INVALID,
		SPHERE,
		BOX,
		CYLINDER,
		MESH,
	};

	public enum class PhysicsFlag
	{
		NONE,
		DISABLE_GRAVITY,
		ENABLE_GYROSCOPIC_FORCE,
	};

	public ref class WCustomPhysicsComponent : public WGameComponent
	{
	public:

		WCustomPhysicsComponent();
		WCustomPhysicsComponent( CustomPhysicsComponent* component );

		DECLARE_CAST_COMPONENT( CustomPhysicsComponent );

		DECLARE_PROPERTY( Sentinel::Components::ShapeType, ShapeType );
		DECLARE_PROPERTY( float,		Mass );

		DECLARE_PROPERTY( WVector3f^,	ShapePosition );
		DECLARE_PROPERTY( WQuatf^,		ShapeOrientation );
		DECLARE_PROPERTY( WVector3f^,	ShapeScale );

		DECLARE_PROPERTY( int,			Flags );
		DECLARE_PROPERTY( float,		LinearDamping );
		DECLARE_PROPERTY( float,		AngularDamping );
		DECLARE_PROPERTY( float,		Restitution );
		DECLARE_PROPERTY( float,		Friction );

		DECLARE_PROPERTY( WVector3f^,	AngularFactor );
		DECLARE_PROPERTY( WVector3f^,	Gravity );
	};
}}
