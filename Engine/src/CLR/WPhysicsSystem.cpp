#pragma once

#include "Property.h"
#include "WPhysicsSystem.h"
#include "WMesh.h"
#include "WQuatf.h"
#include "WVector3f.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_PTR( RigidBody );

	WRigidBody::WRigidBody( RigidBody* body )
	{
		mRef = body;
	}

	void WRigidBody::Release()
	{
		delete mRef;
	}

	DEFINE_OP_PTR( RigidBody );

	DEFINE_PROPERTY_F(   RigidBody, Vector3f,			Position );
	DEFINE_PROPERTY_F(   RigidBody, Quatf,				Orientation );
	DEFINE_PROPERTY_F(   RigidBody, Vector3f,			Scale );

	//////////////////////////////////

	DEFINE_PROPERTY_FM(  RigidBody, float,				Mass );
	DEFINE_PROPERTY_FE(  RigidBody, PhysicsShapeType,	ShapeType );
	DEFINE_PROPERTY_FM(  RigidBody, int,				Flags );
	
	DEFINE_PROPERTY_FMG( RigidBody, float,				LinearDamping );
	DEFINE_PROPERTY_FMG( RigidBody, float,				AngularDamping );

	void WRigidBody::SetDamping( float linear, float angular )
	{
		mRef->SetDamping( linear, angular );
	}

	DEFINE_PROPERTY_FM(  RigidBody, float,				Restitution );
	DEFINE_PROPERTY_FM(  RigidBody, float,				Friction );
	
	DEFINE_PROPERTY_F(   RigidBody, Vector3f,			AngularFactor );
	DEFINE_PROPERTY_F(   RigidBody, Vector3f,			Gravity );
	
	//////////////////////////////////

	void WRigidBody::ApplyCentralImpulse( WVector3f^ impulse )
	{
		mRef->ApplyCentralImpulse( impulse );
	}
	
	////////////////////////////////////////////////////////////////////

	DEFINE_REF_PTR( PhysicsSystem );

	WPhysicsSystem::WPhysicsSystem( PhysicsSystem* physics )
	{
		mRef = physics;
	}

	void WPhysicsSystem::Release()
	{
		delete mRef;
	}

	DEFINE_OP_PTR( PhysicsSystem );

	void WPhysicsSystem::Startup()
	{
		mRef->Startup();
	}

	void WPhysicsSystem::Update( float DT )
	{
		mRef->Update( DT );
	}

	void WPhysicsSystem::Shutdown()
	{
		mRef->Shutdown();
	}

	//////////////////////////////////

	WRigidBody^ WPhysicsSystem::CreateSphere( WVector3f^ position, WQuatf^ orientation, float radius, float mass )
	{
		return gcnew WRigidBody( mRef->CreateSphere( position, orientation, radius, mass ));
	}

	WRigidBody^ WPhysicsSystem::CreateBox( WVector3f^ position, WQuatf^ orientation, WVector3f^ scale, float mass )
	{
		return gcnew WRigidBody( mRef->CreateBox( position, orientation, scale, mass ));
	}

	WRigidBody^ WPhysicsSystem::CreateCylinder( WVector3f^ position, WQuatf^ orientation, WVector3f^ scale, float mass )
	{
		return gcnew WRigidBody( mRef->CreateCylinder( position, orientation, scale, mass ));
	}

	WRigidBody^ WPhysicsSystem::CreateMesh( WVector3f^ position, WQuatf^ orientation, WVector3f^ scale, WMesh^ mesh, float mass )
	{
		return gcnew WRigidBody( mRef->CreateMesh( position, orientation, scale, mesh->GetRef().get(), mass ));
	}

	//////////////////////////////////

	void WPhysicsSystem::AddRigidBody( WRigidBody^ body )
	{
		mRef->AddRigidBody( body );
	}

	void WPhysicsSystem::RemoveRigidBody( WRigidBody^ body )
	{
		mRef->RemoveRigidBody( body );
	}
	
	////////////////////////////////////////////////////////////////////

	WPhysicsSystem^ WPhysicsSystem::BuildBT()
	{
		return gcnew WPhysicsSystem( BuildPhysicsSystemBT() );
	}

	WPhysicsSystem^ WPhysicsSystem::BuildSE()
	{
		return gcnew WPhysicsSystem( BuildPhysicsSystemSE() );
	}
}}
