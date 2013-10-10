#include "WPhysicsComponent.h"
#include "PhysicsComponent.cpp"

namespace Sentinel { namespace Components
{
	WPhysicsComponent::WPhysicsComponent()
	{
		mRef = new PhysicsComponent();
	}

	WPhysicsComponent::WPhysicsComponent( PhysicsComponent* component )
	{
		mRef = component;
	}

	DEFINE_CAST_COMPONENT( PhysicsComponent );
	
	Sentinel::Components::PhysicsShapeType WPhysicsComponent::ShapeType::get()
	{
		return (Sentinel::Components::PhysicsShapeType)static_cast< PhysicsComponent* >(mRef)->mShapeType;
	}

	void WPhysicsComponent::ShapeType::set( Sentinel::Components::PhysicsShapeType v )
	{
		static_cast< PhysicsComponent* >(mRef)->mShapeType = (Sentinel::PhysicsComponent::ShapeType)v;
	}

	DEFINE_PROPERTY_MS( PhysicsComponent, float,		Mass );

	DEFINE_PROPERTY_RS( PhysicsComponent, Vector3f,		ShapePosition );
	DEFINE_PROPERTY_RS( PhysicsComponent, Quatf,		ShapeOrientation );
	DEFINE_PROPERTY_RS( PhysicsComponent, Vector3f,		ShapeScale );

	DEFINE_PROPERTY_MS( PhysicsComponent, int,			Flags );
	DEFINE_PROPERTY_MS( PhysicsComponent, float,		LinearDamping );
	DEFINE_PROPERTY_MS( PhysicsComponent, float,		AngularDamping );
	DEFINE_PROPERTY_MS( PhysicsComponent, float,		Restitution );
	DEFINE_PROPERTY_MS( PhysicsComponent, float,		Friction );

	DEFINE_PROPERTY_RS( PhysicsComponent, Vector3f,		AngularFactor );
	DEFINE_PROPERTY_RS( PhysicsComponent, Vector3f,		Gravity );
}}
