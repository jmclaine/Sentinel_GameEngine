#include "PhysicsShape.h"

namespace Sentinel
{
	PhysicsShape::~PhysicsShape()
	{}

	PhysicsShape::Type PhysicsShape::GetType()
	{
		return mType;
	}

	void* PhysicsShape::GetData()
	{
		return this;
	}

	void PhysicsShape::Save( Archive& archive )
	{
		BYTE type = mType;
		archive.Write( &type );
	}

	///////////////////////////////////////////////

	SpherePhysicsShape::SpherePhysicsShape()
	{
		mType = SPHERE;
	}

	SpherePhysicsShape::~SpherePhysicsShape()
	{}

	void SpherePhysicsShape::Save( Archive& archive )
	{
		PhysicsShape::Save( archive );

		float radius = GetRadius();
		archive.Write( &radius );
	}

	void SpherePhysicsShape::Load( Archive& archive )
	{
		float radius;
		archive.Read( &radius );

		Create( radius );
	}

	///////////////////////////////////////////////

	BoxPhysicsShape::BoxPhysicsShape()
	{
		mType = BOX;
	}

	BoxPhysicsShape::~BoxPhysicsShape()
	{}

	void BoxPhysicsShape::Save( Archive& archive )
	{
		PhysicsShape::Save( archive );

		Vector3f scale( GetScale() );
		archive.Write( scale.Ptr(), ar_sizeof( scale ));
	}

	void BoxPhysicsShape::Load( Archive& archive )
	{
		Vector3f scale;
		archive.Read( scale.Ptr(), ar_sizeof( scale ));

		Create( scale );
	}

	///////////////////////////////////////////////

	CylinderPhysicsShape::CylinderPhysicsShape()
	{
		mType = CYLINDER;
	}

	CylinderPhysicsShape::~CylinderPhysicsShape()
	{}

	void CylinderPhysicsShape::Save( Archive& archive )
	{
		PhysicsShape::Save( archive );

		Vector3f scale( GetScale() );
		archive.Write( scale.Ptr(), ar_sizeof( scale ));
	}

	void CylinderPhysicsShape::Load( Archive& archive )
	{
		Vector3f scale;
		archive.Read( scale.Ptr(), ar_sizeof( scale ));

		Create( scale );
	}

	///////////////////////////////////////////////

	MeshPhysicsShape::MeshPhysicsShape()
	{
		mType = MESH;
	}

	MeshPhysicsShape::~MeshPhysicsShape()
	{}

	void MeshPhysicsShape::Save( Archive& archive )
	{
		PhysicsShape::Save( archive );

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
		mType = MESH;

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
	{
		mType  = COMPOUND;
	}

	CompoundPhysicsShape::~CompoundPhysicsShape()
	{}

	void CompoundPhysicsShape::Save( Archive& archive )
	{
		PhysicsShape::Save( archive );
	}

	void CompoundPhysicsShape::Load( Archive& archive )
	{}
}
