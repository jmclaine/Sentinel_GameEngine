#pragma once

#include "Sentinel.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"

namespace Sentinel
{
	class Plane;
	class Triangle;
	class BoundingSphere;
	class BoundingBox;

	class SENTINEL_DLL Ray
	{
	public:
		Vector3 mPosition;
		Vector3 mDirection;

		Ray(const Vector3& pos, const Vector3& dir);
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL Plane
	{
	public:
		Vector3 mPosition;
		Vector3 mNormal;

		Plane(const Vector3& pos = Vector3(), const Vector3& normal = Vector3());

		float Distance(const Vector3& pos) const;

		//bool Intersects(const Ray& ray) const;
		//bool Intersects(const BoundingBox& box) const;
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL Triangle
	{
	public:
		Vector3 mPosition[3];

		Triangle(const Vector3& posA, const Vector3& posB, const Vector3& posC);

		float Area();
	};

	//////////////////////////////////////////////////////

	class BoundingShape
	{
	public:
		virtual bool Intersects(const Vector3& point) const = 0;
		virtual bool Intersects(const Ray& ray, Vector3* intersection = nullptr) const = 0;
		virtual bool Intersects(const BoundingSphere& sphere) const = 0;
		virtual bool Intersects(const BoundingBox& box) const = 0;
	};

	//////////////////////////////////////////////////////

	class SENTINEL_DLL BoundingSphere : public BoundingShape
	{
	public:
		Vector3 mCenter;
		float mRadius;

		BoundingSphere(const Vector3& center = Vector3(), float radius = 0);
		BoundingSphere(const Vector3& v0, const Vector3& v1);
		BoundingSphere(const Vector3& v0, const Vector3& v1, const Vector3& v2);
		BoundingSphere(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3);
		BoundingSphere(const BYTE* verts, UINT count, UINT stride = sizeof(Vector3));	// Takes in a pointer to 3 float points.

		bool Intersects(const Vector3& point) const;
		bool Intersects(const Ray& ray, Vector3* intersection = nullptr) const;
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
		BoundingBox(const Vector3& minBounds, const Vector3& maxBounds);
		BoundingBox(const BYTE* verts, UINT count, UINT stride = sizeof(Vector3));

		void Set(const Matrix4x4& matWorld = Matrix4x4::IDENTITY);
		void Set(const Vector3& minBounds, const Vector3& maxBounds, const Matrix4x4& matWorld = Matrix4x4::IDENTITY);
		void Set(const BYTE* verts, UINT count, UINT stride = sizeof(Vector3));

		const Vector3& GetMinBounds() const;
		const Vector3& GetMaxBounds() const;

		bool Intersects(const Vector3& point) const;
		bool Intersects(const Ray& ray, Vector3* intersection = nullptr) const;
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
			const Vector3& nearCenter, const Vector3& farCenter,
			const Vector2& nearExtent, const Vector2& farExtent);

		BoundingFrustum(
			const Vector3& nearCenter, const Vector3& farCenter,
			const Vector2& nearExtent, const Vector2& farExtent,
			const Vector3& forward, const Vector3& right, const Vector3& up);

		void Set(
			const Vector3& nearCenter, const Vector3& farCenter,
			const Vector2& nearExtent, const Vector2& farExtent);

		void Set(
			const Vector3& nearCenter, const Vector3& farCenter,
			const Vector2& nearExtent, const Vector2& farExtent,
			const Vector3& forward, const Vector3& right, const Vector3& up);

		bool Intersects(const Vector3& point) const;
		bool Intersects(const Ray& ray, Vector3* intersection = nullptr) const;
		bool Intersects(const BoundingSphere& sphere) const;
		bool Intersects(const BoundingBox& box) const;
	};
}
