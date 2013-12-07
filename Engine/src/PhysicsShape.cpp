#include "PhysicsShape.h"
#include "PhysicsSystem.h"

namespace Sentinel
{
	PhysicsShape::~PhysicsShape()
	{}

	void* PhysicsShape::GetData()
	{
		return this;
	}

	///////////////////////////////////////////////

	SpherePhysicsShape::SpherePhysicsShape()
	{}

	SpherePhysicsShape::~SpherePhysicsShape()
	{}

	void SpherePhysicsShape::Save( Archive& archive )
	{
		PhysicsSystem::SERIAL_CreateSphere.Save( archive );

		float radius = GetRadius();
		archive.Write( &radius );
	}

	void SpherePhysicsShape::Load( Archive& archive )
	{
		float radius;
		archive.Read( &radius );

		SetRadius( radius );
	}

	///////////////////////////////////////////////

	BoxPhysicsShape::BoxPhysicsShape()
	{}

	BoxPhysicsShape::~BoxPhysicsShape()
	{}

	void BoxPhysicsShape::Save( Archive& archive )
	{
		PhysicsSystem::SERIAL_CreateBox.Save( archive );

		Vector3f scale( GetScale() );
		archive.Write( scale.Ptr(), ar_sizeof( scale ));
	}

	void BoxPhysicsShape::Load( Archive& archive )
	{
		Vector3f scale;
		archive.Read( scale.Ptr(), ar_sizeof( scale ));

		SetScale( scale );
	}

	///////////////////////////////////////////////

	CylinderPhysicsShape::CylinderPhysicsShape()
	{}

	CylinderPhysicsShape::~CylinderPhysicsShape()
	{}

	void CylinderPhysicsShape::Save( Archive& archive )
	{
		PhysicsSystem::SERIAL_CreateCylinder.Save( archive );

		Vector3f scale( GetScale() );
		archive.Write( scale.Ptr(), ar_sizeof( scale ));
	}

	void CylinderPhysicsShape::Load( Archive& archive )
	{
		Vector3f scale;
		archive.Read( scale.Ptr(), ar_sizeof( scale ));

		SetScale( scale );
	}

	///////////////////////////////////////////////

	MeshPhysicsShape::MeshPhysicsShape()
	{}

	MeshPhysicsShape::~MeshPhysicsShape()
	{}

	void MeshPhysicsShape::Save( Archive& archive )
	{
		PhysicsSystem::SERIAL_CreateMesh.Save( archive );

		int count = GetNumPoints();
		archive.Write( &count );

		Vector3f* verts = GetPoints();
		for( int x = 0; x < count; ++x )
		{
			archive.Write( &verts[ x ].x );
			archive.Write( &verts[ x ].y );
			archive.Write( &verts[ x ].z );
		}

		Vector3f& scale = GetScale();
		archive.Write( scale.Ptr(), ar_sizeof( scale ));
	}

	void MeshPhysicsShape::Load( Archive& archive )
	{
		int count;
		archive.Read( &count );

		Vector3f v;
		for( int x = 0; x < count; ++x )
		{
			archive.Read( v.Ptr(), ar_sizeof( v ));
			AddPoint( v );
		}

		archive.Read( v.Ptr(), ar_sizeof( v ));
		SetScale( v );
	}

	///////////////////////////////////////////////

	CompoundPhysicsShape::CompoundPhysicsShape()
	{}

	CompoundPhysicsShape::~CompoundPhysicsShape()
	{}

	void CompoundPhysicsShape::Save( Archive& archive )
	{}

	void CompoundPhysicsShape::Load( Archive& archive )
	{}
}
