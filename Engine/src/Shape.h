#pragma once

#include "Sentinel.h"
#include "Types.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Matrix4f.h"

namespace Sentinel
{
	class Plane;
	class Triangle;
	class BoundingSphere;
	class BoundingBox;

	class SENTINEL_DLL Ray
	{
	public:

		Vector3f mPosition;
		Vector3f mDirection;

		Ray(const Vector3f& pos, const Vector3f& dir);
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL Plane
	{
	public:

		Vector3f mPosition;
		Vector3f mNormal;

		Plane(const Vector3f& pos = Vector3f(), const Vector3f& normal = Vector3f());

		float Distance(const Vector3f& pos) const;

		//bool Intersects(const Ray& ray) const;
		//bool Intersects(const BoundingBox& box) const;
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL Triangle
	{
	public:

		Vector3f mPosition[3];

		Triangle(const Vector3f& posA, const Vector3f& posB, const Vector3f& posC);

		float Area();
	};

	//////////////////////////////////////////////////////

	class BoundingShape
	{
	public:

		virtual bool Intersects(const Vector3f& point) const = 0;
		virtual bool Intersects(const Ray& ray, Vector3f* intersection = NULL) const = 0;
		virtual bool Intersects(const BoundingSphere& sphere) const = 0;
		virtual bool Intersects(const BoundingBox& box) const = 0;
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL BoundingSphere : public BoundingShape
	{
	public:

		Vector3f mCenter;
		float mRadius;

		BoundingSphere(const Vector3f& center = Vector3f(), float radius = 0);
		BoundingSphere(const Vector3f& v0, const Vector3f& v1);
		BoundingSphere(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2);
		BoundingSphere(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
		BoundingSphere(const BYTE* verts, UINT count, UINT stride = sizeof(Vector3f));	// Takes in a pointer to 3 float points.

		bool Intersects(const Vector3f& point) const;
		bool Intersects(const Ray& ray, Vector3f* intersection = NULL) const;
		bool Intersects(const BoundingSphere& sphere) const;
		bool Intersects(const BoundingBox& box) const;
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL BoundingBox : public BoundingShape
	{
	private:

		Plane mPlane[6];

	public:

		BoundingBox();
		BoundingBox(const Vector3f& minBounds, const Vector3f& maxBounds);
		BoundingBox(const BYTE* verts, UINT count, UINT stride = sizeof(Vector3f));

		void Set(const Matrix4f& matWorld = Matrix4f::IDENTITY);
		void Set(const Vector3f& minBounds, const Vector3f& maxBounds, const Matrix4f& matWorld = Matrix4f::IDENTITY);
		void Set(const BYTE* verts, UINT count, UINT stride = sizeof(Vector3f));

		const Vector3f& GetMinBounds() const;
		const Vector3f& GetMaxBounds() const;

		bool Intersects(const Vector3f& point) const;
		bool Intersects(const Ray& ray, Vector3f* intersection = NULL) const;
		bool Intersects(const BoundingSphere& sphere) const;
		bool Intersects(const BoundingBox& box) const;
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL BoundingFrustum : public BoundingShape
	{
	private:

		Plane mPlane[6];

	public:

		BoundingFrustum();

		BoundingFrustum(
			const Vector3f& nearCenter, const Vector3f& farCenter,
			const Vector2f& nearExtent, const Vector2f& farExtent);

		BoundingFrustum(
			const Vector3f& nearCenter, const Vector3f& farCenter,
			const Vector2f& nearExtent, const Vector2f& farExtent,
			const Vector3f& forward, const Vector3f& right, const Vector3f& up);

		void Set(
			const Vector3f& nearCenter, const Vector3f& farCenter,
			const Vector2f& nearExtent, const Vector2f& farExtent);

		void Set(
			const Vector3f& nearCenter, const Vector3f& farCenter,
			const Vector2f& nearExtent, const Vector2f& farExtent,
			const Vector3f& forward, const Vector3f& right, const Vector3f& up);

		bool Intersects(const Vector3f& point) const;
		bool Intersects(const Ray& ray, Vector3f* intersection = NULL) const;
		bool Intersects(const BoundingSphere& sphere) const;
		bool Intersects(const BoundingBox& box) const;
	};
}
