#pragma once

#include "Sentinel.h"
#include "Archive.h"
#include "Serializable.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Sentinel
{
	class PhysicsSystem;

	class PhysicsShape : public Serializable
	{
	public:

		virtual ~PhysicsShape();

		virtual void* GetData();

		virtual void Save(Archive& archive) = 0;
		virtual void Load(Archive& archive) = 0;

		virtual PhysicsShape* Copy(PhysicsSystem* physics) = 0;
	};

	///////////////////////////////////////////////

	class SpherePhysicsShape : public PhysicsShape
	{
	protected:

		SpherePhysicsShape();

	public:

		virtual ~SpherePhysicsShape();

	protected:

		void Create(float radius);

	public:

		virtual float GetRadius() = 0;
		virtual void SetRadius(float radius) = 0;

		void Save(Archive& archive);
		void Load(Archive& archive);

		PhysicsShape* Copy(PhysicsSystem* physics);
	};

	///////////////////////////////////////////////

	class BoxPhysicsShape : public PhysicsShape
	{
	protected:

		BoxPhysicsShape();

	public:

		virtual ~BoxPhysicsShape();

	protected:

		void Create(const Vector3& scale);

	public:

		virtual Vector3 GetScale() = 0;
		virtual void SetScale(const Vector3& scale) = 0;

		void Save(Archive& archive);
		void Load(Archive& archive);

		PhysicsShape* Copy(PhysicsSystem* physics);
	};

	///////////////////////////////////////////////

	class CylinderPhysicsShape : public PhysicsShape
	{
	protected:

		CylinderPhysicsShape();

	public:

		virtual ~CylinderPhysicsShape();

	protected:

		void Create(const Vector3& scale);

	public:

		virtual Vector3 GetScale() = 0;
		virtual void SetScale(const Vector3& scale) = 0;

		void Save(Archive& archive);
		void Load(Archive& archive);

		PhysicsShape* Copy(PhysicsSystem* physics);
	};

	///////////////////////////////////////////////

	class MeshPhysicsShape : public PhysicsShape
	{
	protected:

		MeshPhysicsShape();

	public:

		virtual ~MeshPhysicsShape();

	protected:

		void Create(void* verts, UINT count, UINT stride, const Vector3& scale);

	public:

		virtual int GetNumPoints() = 0;
		virtual void AddPoint(const Vector3& point) = 0;
		virtual Vector3* GetPoints() = 0;

		virtual Vector3 GetScale() = 0;
		virtual void SetScale(const Vector3& scale) = 0;

		void Save(Archive& archive);
		void Load(Archive& archive);

		PhysicsShape* Copy(PhysicsSystem* physics);
	};

	///////////////////////////////////////////////

	class CompoundPhysicsShape : public PhysicsShape
	{
	protected:

		CompoundPhysicsShape();

	public:

		virtual ~CompoundPhysicsShape();

		virtual void Save(Archive& archive);
		virtual void Load(Archive& archive);

		PhysicsShape* Copy(PhysicsSystem* physics);
	};
}
