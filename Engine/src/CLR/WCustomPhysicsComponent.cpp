#include "WCustomPhysicsComponent.h"

namespace Sentinel { namespace Components
{
	WCustomPhysicsComponent::WCustomPhysicsComponent()
	{
		mRef = new CustomPhysicsComponent();
	}

	WCustomPhysicsComponent::WCustomPhysicsComponent( CustomPhysicsComponent* component )
	{
		mRef = component;
	}

	DEFINE_CAST_COMPONENT( CustomPhysicsComponent );
	
	DEFINE_PROPERTY_ES( CustomPhysicsComponent, Components, ShapeType, ShapeType );
	DEFINE_PROPERTY_MS( CustomPhysicsComponent, float,		Mass );

	DEFINE_PROPERTY_RS( CustomPhysicsComponent, Vector3f,	ShapePosition );
	DEFINE_PROPERTY_RS( CustomPhysicsComponent, Quatf,		ShapeOrientation );
	DEFINE_PROPERTY_RS( CustomPhysicsComponent, Vector3f,	ShapeScale );

	DEFINE_PROPERTY_MS( CustomPhysicsComponent, int,		Flags );
	DEFINE_PROPERTY_MS( CustomPhysicsComponent, float,		LinearDamping );
	DEFINE_PROPERTY_MS( CustomPhysicsComponent, float,		AngularDamping );
	DEFINE_PROPERTY_MS( CustomPhysicsComponent, float,		Restitution );
	DEFINE_PROPERTY_MS( CustomPhysicsComponent, float,		Friction );

	DEFINE_PROPERTY_RS( CustomPhysicsComponent, Vector3f,	AngularFactor );
	DEFINE_PROPERTY_RS( CustomPhysicsComponent, Vector3f,	Gravity );
}}
