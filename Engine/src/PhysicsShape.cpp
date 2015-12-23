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

	void SpherePhysicsShape::Create(float radius)
	{
		SetRadius(radius);
	}

	void SpherePhysicsShape::Save(Archive& archive)
	{
		PhysicsSystem::SERIAL_CreateSphere.Save(archive);

		float radius = GetRadius();
		archive.Write(&radius);
	}

	void SpherePhysicsShape::Load(Archive& archive)
	{
		float radius;
		archive.Read(&radius);

		SetRadius(radius);
	}

	PhysicsShape* SpherePhysicsShape::Copy(PhysicsSystem* physics)
	{
		return physics->CreateSphere(GetRadius());
	}

	///////////////////////////////////////////////

	BoxPhysicsShape::BoxPhysicsShape()
	{}

	BoxPhysicsShape::~BoxPhysicsShape()
	{}

	void BoxPhysicsShape::Create(const Vector3& scale)
	{
		SetScale(scale);
	}

	void BoxPhysicsShape::Save(Archive& archive)
	{
		PhysicsSystem::SERIAL_CreateBox.Save(archive);

		Vector3 scale(GetScale());
		archive.Write(scale.Ptr(), ar_sizeof(scale));
	}

	void BoxPhysicsShape::Load(Archive& archive)
	{
		Vector3 scale;
		archive.Read(scale.Ptr(), ar_sizeof(scale));

		SetScale(scale);
	}

	PhysicsShape* BoxPhysicsShape::Copy(PhysicsSystem* physics)
	{
		return physics->CreateBox(GetScale());
	}

	///////////////////////////////////////////////

	CylinderPhysicsShape::CylinderPhysicsShape()
	{}

	CylinderPhysicsShape::~CylinderPhysicsShape()
	{}

	void CylinderPhysicsShape::Create(const Vector3& scale)
	{
		SetScale(scale);
	}

	void CylinderPhysicsShape::Save(Archive& archive)
	{
		PhysicsSystem::SERIAL_CreateCylinder.Save(archive);

		Vector3 scale(GetScale());
		archive.Write(scale.Ptr(), ar_sizeof(scale));
	}

	void CylinderPhysicsShape::Load(Archive& archive)
	{
		Vector3 scale;
		archive.Read(scale.Ptr(), ar_sizeof(scale));

		SetScale(scale);
	}

	PhysicsShape* CylinderPhysicsShape::Copy(PhysicsSystem* physics)
	{
		return physics->CreateCylinder(GetScale());
	}

	///////////////////////////////////////////////

	MeshPhysicsShape::MeshPhysicsShape()
	{}

	MeshPhysicsShape::~MeshPhysicsShape()
	{}

	void MeshPhysicsShape::Create(void* verts, UINT count, UINT stride, const Vector3& scale)
	{
		BYTE* data = (BYTE*)verts;

		for (UINT x = 0; x < count; ++x)
		{
			AddPoint(*(Vector3*)data);

			data += stride;
		}

		SetScale(scale);
	}

	void MeshPhysicsShape::Save(Archive& archive)
	{
		PhysicsSystem::SERIAL_CreateMesh.Save(archive);

		int count = GetNumPoints();
		archive.Write(&count);

		Vector3* verts = GetPoints();
		for (int x = 0; x < count; ++x)
		{
			archive.Write(&verts[x].x);
			archive.Write(&verts[x].y);
			archive.Write(&verts[x].z);
		}

		Vector3& scale = GetScale();
		archive.Write(scale.Ptr(), ar_sizeof(scale));
	}

	void MeshPhysicsShape::Load(Archive& archive)
	{
		int count;
		archive.Read(&count);

		Vector3 v;
		for (int x = 0; x < count; ++x)
		{
			archive.Read(v.Ptr(), ar_sizeof(v));
			AddPoint(v);
		}

		archive.Read(v.Ptr(), ar_sizeof(v));
		SetScale(v);
	}

	PhysicsShape* MeshPhysicsShape::Copy(PhysicsSystem* physics)
	{
		return physics->CreateMesh(GetPoints(), GetNumPoints(), sizeof(Vector3), GetScale());
	}

	///////////////////////////////////////////////

	CompoundPhysicsShape::CompoundPhysicsShape()
	{}

	CompoundPhysicsShape::~CompoundPhysicsShape()
	{}

	void CompoundPhysicsShape::Save(Archive& archive)
	{}

	void CompoundPhysicsShape::Load(Archive& archive)
	{}

	PhysicsShape* CompoundPhysicsShape::Copy(PhysicsSystem* physics)
	{
		return NULL;
	}
}
