#include <vector>

#include "MathUtil.h"
#include "Shape.h"
#include "Vector4.h"
#include "Debug.h"

namespace Sentinel
{
	Ray::Ray(const Vector3& pos, const Vector3& dir) :
		mPosition(pos),
		mDirection(dir)
	{ }

	/////////////////////////////////////////////////////////////////////////////////////

	Plane::Plane(const Vector3& pos, const Vector3& normal) :
		mPosition(pos),
		mNormal(normal)
	{ }

	float Plane::Distance(const Vector3& pos) const
	{
		return mNormal.Dot(pos) - mPosition.Dot(mNormal);
	}

	/////////////////////////////////////////////////////////////////////////////////////

	Triangle::Triangle(
		const Vector3& posA, 
		const Vector3& posB, 
		const Vector3& posC)
	{
		mPosition[0] = posA;
		mPosition[1] = posB;
		mPosition[2] = posC;
	}

	// Based on Heron's Formula @ www.mathopenref.com/heronsformula.html
	//
	float Triangle::Area()
	{
		float a = (mPosition[0] - mPosition[1]).Length();
		float b = (mPosition[0] - mPosition[2]).Length();
		float c = (mPosition[1] - mPosition[2]).Length();
		float p = (a + b + c) / 2;

		return sqrt(p * (p - a) * (p - b) * (p - c));
	}

	/////////////////////////////////////////////////////////////////////////////////////

	BoundingSphere::BoundingSphere(const Vector3& center, float radius) :
		mCenter(center),
		mRadius(radius)
	{ }

	// Create a sphere based on points contained within it.
	// Based on code by:
	// Nicolas Capens @ www.flipcode.com/archives/Smallest_Enclosing_Spheres.html
	//
	BoundingSphere::BoundingSphere(
		const Vector3& v0, 
		const Vector3& v1)
	{
		Vector3 u1 = v1 - v0;
		Vector3 u0 = u1 * 0.5f;

		mRadius = u0.Length() + (float)EPSILON;
		mCenter = v0 + u0;
	}

	BoundingSphere::BoundingSphere(
		const Vector3& v0, 
		const Vector3& v1, 
		const Vector3& v2)
	{
		Vector3 u1 = v1 - v0;
		Vector3 u2 = v2 - v0;

		float det = 2.0f * u1.Cross(u2).Dot(u1.Cross(u2));

		// Guarantee a smallest sphere of two points.
		//
		if (det == 0)
		{
			BoundingSphere sphere;

			float len01 = u1.LengthSquared();
			float len02 = u2.LengthSquared();

			if (len01 > len02)
			{
				sphere = BoundingSphere(v0, v1);
			}
			else
			{
				sphere = BoundingSphere(v0, v2);
			}

			mRadius = sphere.mRadius;
			mCenter = sphere.mCenter;

			return;
		}

		Vector3 u0 = (u1.Cross(u2).Cross(u1) * u2.LengthSquared() + \
			u2.Cross(u1.Cross(u2)) * u1.LengthSquared()) * (1.0f / det);

		mRadius = u0.Length() + (float)EPSILON;
		mCenter = v0.Add(u0);
	}

	BoundingSphere::BoundingSphere(
		const Vector3& v0, 
		const Vector3& v1, 
		const Vector3& v2, 
		const Vector3& v3)
	{
		Vector3 u1 = v1 - v0;
		Vector3 u2 = v2 - v0;
		Vector3 u3 = v3 - v0;

		float m[9];
		m[0] = u1.x;	m[1] = u1.y;	m[2] = u1.z;
		m[3] = u2.x;	m[4] = u2.y;	m[5] = u2.z;
		m[6] = u3.x;	m[7] = u3.y;	m[8] = u3.z;

		float det = 
			2.0f * m[0] * (m[4] * m[8] - m[5] * m[7]) - 
			m[1] * (m[3] * m[8] - m[5] * m[6]) + 
			m[2] * (m[3] * m[7] - m[4] * m[6]);

		// Guarantee a smallest sphere of three points.
		//
		if (det == 0)
		{
			BoundingSphere sphere;
			float tri012 = Triangle(v0, v1, v2).Area();
			float tri013 = Triangle(v0, v1, v3).Area();
			float tri023 = Triangle(v0, v2, v3).Area();

			if (tri012 > tri013 && tri012 > tri023)
			{
				sphere = BoundingSphere(v0, v1, v2);
			}
			else if (tri013 > tri012 && tri013 > tri023)
			{
				sphere = BoundingSphere(v0, v1, v3);
			}
			else
			{
				sphere = BoundingSphere(v0, v2, v3);
			}

			mRadius = sphere.mRadius;
			mCenter = sphere.mCenter;
			return;
		}

		Vector3 u0 = (u1.Cross(u2) * u3.LengthSquared() + \
			u3.Cross(u1) * u2.LengthSquared() + \
			u2.Cross(u3) * u1.LengthSquared()) * (1.0f / det);

		mRadius = u0.Length() + (float)EPSILON;
		mCenter = v0 + u0;
	}

	// Find the smallest sphere.
	// Based on code by:
	// Nicolas Capens @ www.flipcode.com/archives/Smallest_Enclosing_Spheres.html
	// Guest @ pastebin.com/04b1GBA2
	//
	BoundingSphere::BoundingSphere(const BYTE* verts, UINT count, UINT stride)
	{
		std::vector<Vector3> points; // points of interest
		mCenter = *(Vector3*)verts;

		points.push_back(mCenter);

		Vector3 pos;
		UINT currIndex = 0;

		while (currIndex < count)
		{
			pos = *(Vector3*)(verts + currIndex * stride);

			if (!Intersects(pos))
			{
				BoundingSphere minSphere;
				BoundingSphere newSphere;
				int index = -1;

				switch (points.size())
				{
				case 1:
					minSphere = BoundingSphere(pos, points[0]);
					points.push_back(pos);
					break;

				case 2:
					newSphere = BoundingSphere(pos, points[0]);
					if (newSphere.Intersects(points[1]))
					{
						minSphere = newSphere;
						index = 0;
					}

					newSphere = BoundingSphere(pos, points[1]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]))
					{
						minSphere = newSphere;
						index = 1;
					}

					if (index > -1)
					{
						points[1 - index] = pos;
					}
					else
					{
						minSphere = BoundingSphere(pos, points[0], points[1]);
						points.push_back(pos);
					}
					break;

				case 3:
					newSphere = BoundingSphere(pos, points[0]);
					if (newSphere.Intersects(points[1]) &&
						newSphere.Intersects(points[2]))
					{
						minSphere = newSphere;
						index = 0;
					}

					newSphere = BoundingSphere(pos, points[1]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]) &&
						newSphere.Intersects(points[2]))
					{
						minSphere = newSphere;
						index = 1;
					}

					newSphere = BoundingSphere(pos, points[2]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]) &&
						newSphere.Intersects(points[1]))
					{
						minSphere = newSphere;
						index = 2;
					}

					newSphere = BoundingSphere(pos, points[0], points[1]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[2]))
					{
						minSphere = newSphere;
						index = 3;
					}

					newSphere = BoundingSphere(pos, points[0], points[2]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[1]))
					{
						minSphere = newSphere;
						index = 4;
					}

					newSphere = BoundingSphere(pos, points[1], points[2]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]))
					{
						minSphere = newSphere;
						index = 5;
					}

					switch (index)
					{
					case 0:
						points.resize(2);
						points[1] = pos;
						break;

					case 1:
						points.resize(2);
						points[0] = pos;
						break;

					case 2:
						points[0] = points[2];
						points.resize(2);
						points[1] = pos;
						break;

					case 3:
						points[2] = pos;
						break;

					case 4:
						points[1] = pos;
						break;

					case 5:
						points[0] = pos;
						break;

					default:
						minSphere = BoundingSphere(pos, points[0], points[1], points[2]);
						points.push_back(pos);
						break;
					}
					break;

				case 4:
					// Check 2 point spheres.
					//
					newSphere = BoundingSphere(pos, points[0]);
					if (newSphere.Intersects(points[1]) &&
						newSphere.Intersects(points[2]) &&
						newSphere.Intersects(points[3]))
					{
						minSphere = newSphere;
						index = 0;
					}

					newSphere = BoundingSphere(pos, points[1]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]) &&
						newSphere.Intersects(points[2]) &&
						newSphere.Intersects(points[3]))
					{
						minSphere = newSphere;
						index = 1;
					}

					newSphere = BoundingSphere(pos, points[2]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]) &&
						newSphere.Intersects(points[1]) &&
						newSphere.Intersects(points[3]))
					{
						minSphere = newSphere;
						index = 2;
					}

					newSphere = BoundingSphere(pos, points[3]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]) &&
						newSphere.Intersects(points[1]) &&
						newSphere.Intersects(points[2]))
					{
						minSphere = newSphere;
						index = 3;
					}

					// Check 3 point spheres.
					//
					newSphere = BoundingSphere(pos, points[0], points[1]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[2]) &&
						newSphere.Intersects(points[3]))
					{
						minSphere = newSphere;
						index = 4;
					}

					newSphere = BoundingSphere(pos, points[0], points[2]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[1]) &&
						newSphere.Intersects(points[3]))
					{
						minSphere = newSphere;
						index = 5;
					}

					newSphere = BoundingSphere(pos, points[0], points[3]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[1]) &&
						newSphere.Intersects(points[2]))
					{
						minSphere = newSphere;
						index = 6;
					}

					newSphere = BoundingSphere(pos, points[1], points[2]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]) &&
						newSphere.Intersects(points[3]))
					{
						minSphere = newSphere;
						index = 7;
					}

					newSphere = BoundingSphere(pos, points[1], points[3]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]) &&
						newSphere.Intersects(points[2]))
					{
						minSphere = newSphere;
						index = 8;
					}

					newSphere = BoundingSphere(pos, points[2], points[3]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]) &&
						newSphere.Intersects(points[1]))
					{
						minSphere = newSphere;
						index = 9;
					}

					// Check 4 point spheres.
					//
					newSphere = BoundingSphere(pos, points[0], points[1], points[2]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[3]))
					{
						minSphere = newSphere;
						index = 10;
					}

					newSphere = BoundingSphere(pos, points[0], points[1], points[3]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[2]))
					{
						minSphere = newSphere;
						index = 11;
					}

					newSphere = BoundingSphere(pos, points[1], points[2], points[3]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[0]))
					{
						minSphere = newSphere;
						index = 12;
					}

					newSphere = BoundingSphere(pos, points[0], points[2], points[3]);
					if (newSphere.mRadius < minSphere.mRadius &&
						newSphere.Intersects(points[1]))
					{
						minSphere = newSphere;
						index = 13;
					}

					switch (index)
					{
					case 0:
						points.resize(2);
						points[1] = pos;
						break;

					case 1:
						points.resize(2);
						points[0] = pos;
						break;

					case 2:
						points[0] = points[2];
						points.resize(2);
						points[1] = pos;
						break;

					case 3:
						points[0] = points[3];
						points.resize(2);
						points[1] = pos;
						break;

					case 4:
						points.resize(3);
						points[2] = pos;
						break;

					case 5:
						points.resize(3);
						points[1] = pos;
						break;

					case 6:
						points[1] = points[3];
						points.resize(3);
						points[2] = pos;
						break;

					case 7:
						points[0] = points[2];
						points.resize(3);
						points[2] = pos;
						break;

					case 8:
						points[0] = points[3];
						points.resize(3);
						points[2] = pos;
						break;

					case 9:
						points[0] = points[3];
						points[1] = pos;
						points.resize(3);
						break;

					case 10:
						points[3] = pos;
						break;

					case 11:
						points[2] = pos;
						break;

					case 12:
						points[0] = pos;
						break;

					case 13:
						points[1] = pos;
						break;
					}
					break;
				}

				if (minSphere.mRadius > mRadius)
				{
					mCenter = minSphere.mCenter;
					mRadius = minSphere.mRadius;

					// Must recheck all points to be certain they
					// all fit within the newly created sphere.
					//
					currIndex = 0;
					continue;
				}
			}

			++currIndex;
		}
	}

	bool BoundingSphere::Intersects(const Vector3& point) const
	{
		return (point - mCenter).LengthSquared() < (mRadius * mRadius);
	}

	bool BoundingSphere::Intersects(const Ray& ray, Vector3* intersection) const
	{
		_ASSERT(0); // not implemented
		return false;
	}

	bool BoundingSphere::Intersects(const BoundingSphere& sphere) const
	{
		float d = (mCenter - sphere.mCenter).LengthSquared();
		float r = (mRadius + sphere.mRadius);

		return (d < r*r);
	}

	bool BoundingSphere::Intersects(const BoundingBox& box) const
	{
		Vector3 closestPointInAabb = mCenter.Max(box.GetMinBounds()).Min(box.GetMaxBounds());

		float distanceSquared = (closestPointInAabb - mCenter).LengthSquared();

		return distanceSquared < (mRadius * mRadius);
	}

	/////////////////////////////////////////////////////////////////////////////////////

	BoundingBox::BoundingBox()
	{
		Set();
	}

	BoundingBox::BoundingBox(
		const Vector3& minBounds, 
		const Vector3& maxBounds)
	{
		Set(minBounds, maxBounds);
	}

	BoundingBox::BoundingBox(const BYTE* verts, UINT count, UINT stride)
	{
		Set(verts, count, stride);
	}

	void BoundingBox::Set(const Matrix4x4& matWorld)
	{
		Set(Vector3(-1, -1, -1), Vector3(1, 1, 1), matWorld);
	}

#define SET_MIN_MAX_POINT(offset)\
	point = matWorld.Transform(center + offset);\
	minPos = minPos.Min(point);\
	maxPos = maxPos.Max(point);

	void BoundingBox::Set(
		const Vector3& minBounds, 
		const Vector3& maxBounds, 
		const Matrix4x4& matWorld)
	{
		Vector3 center((maxBounds + minBounds) * 0.5f);
		Vector3 extent(maxBounds - center);

		Vector3 minPos(FLT_MAX, FLT_MAX, FLT_MAX);
		Vector3 maxPos(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		Vector3 point;
		SET_MIN_MAX_POINT(Vector3(-extent.x, extent.y, -extent.z));
		SET_MIN_MAX_POINT(Vector3(extent.x, extent.y, -extent.z));
		SET_MIN_MAX_POINT(Vector3(extent.x, -extent.y, -extent.z));
		SET_MIN_MAX_POINT(Vector3(-extent.x, -extent.y, -extent.z));
		SET_MIN_MAX_POINT(Vector3(-extent.x, extent.y, extent.z));
		SET_MIN_MAX_POINT(Vector3(extent.x, extent.y, extent.z));
		SET_MIN_MAX_POINT(Vector3(extent.x, -extent.y, extent.z));
		SET_MIN_MAX_POINT(Vector3(-extent.x, -extent.y, extent.z));

		// min
		mPlane[0].mPosition = minPos;
		mPlane[0].mNormal = Vector3(1, 0, 0);

		mPlane[1].mPosition = minPos;
		mPlane[1].mNormal = Vector3(0, 1, 0);

		mPlane[2].mPosition = minPos;
		mPlane[2].mNormal = Vector3(0, 0, 1);

		// max
		mPlane[3].mPosition = maxPos;
		mPlane[3].mNormal = Vector3(-1, 0, 0);

		mPlane[4].mPosition = maxPos;
		mPlane[4].mNormal = Vector3(0, -1, 0);

		mPlane[5].mPosition = maxPos;
		mPlane[5].mNormal = Vector3(0, 0, -1);
	}

	void BoundingBox::Set(const BYTE* verts, UINT count, UINT stride)
	{
		//Debug::Log(STREAM("verts count: " << count << "; stride: " << stride));

		Vector3 minPos(FLT_MAX, FLT_MAX, FLT_MAX);
		Vector3 maxPos(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (UINT x = 0; x < count; ++x)
		{
			const Vector3& vert = *(Vector3*)verts;

			//Debug::Log(vert.ToString());

			minPos = minPos.Min(vert);
			maxPos = maxPos.Max(vert);

			verts += stride;
		}
		
		//Debug::Log("result");
		//Debug::Log(minPos.ToString());
		//Debug::Log(maxPos.ToString());

		Set(minPos, maxPos);
	}

	const Vector3& BoundingBox::GetMinBounds() const
	{
		return mPlane[0].mPosition;
	}

	const Vector3& BoundingBox::GetMaxBounds() const
	{
		return mPlane[3].mPosition;
	}

	bool BoundingBox::Intersects(const Vector3& point) const
	{
		for (UINT x = 0; x < 3; ++x)
			if (point[x] < GetMinBounds()[x] || point[x] > GetMaxBounds()[x])
				return false;

		return true;
	}

	// Check for a Ray being within a Box.
	//
	// Based on code by tavianator
	// http://tavianator.com/2011/05/fast-branchless-raybounding-box-intersections/
	//
	bool BoundingBox::Intersects(const Ray& ray, Vector3* intersection) const
	{
		const Vector3& minBounds = GetMinBounds();
		const Vector3& maxBounds = GetMaxBounds();

		float dx = 1.0f / ray.mDirection.x;
		float dy = 1.0f / ray.mDirection.y;
		float dz = 1.0f / ray.mDirection.z;

		float tx1 = (minBounds.x - ray.mPosition.x)*dx;
		float tx2 = (maxBounds.x - ray.mPosition.x)*dx;

		float tmin = std::fmin(tx1, tx2);
		float tmax = std::fmax(tx1, tx2);

		float ty1 = (minBounds.y - ray.mPosition.y)*dy;
		float ty2 = (maxBounds.y - ray.mPosition.y)*dy;

		tmin = std::fmax(tmin, std::fmin(ty1, ty2));
		tmax = std::fmin(tmax, std::fmax(ty1, ty2));

		float tz1 = (minBounds.z - ray.mPosition.z)*dz;
		float tz2 = (maxBounds.z - ray.mPosition.z)*dz;

		tmin = std::fmax(tmin, std::fmin(tz1, tz2));
		tmax = std::fmin(tmax, std::fmax(tz1, tz2));

		if (intersection != nullptr)
			(*intersection) = ray.mPosition + ray.mDirection * tmin;

		return tmax >= std::fmax(0.0f, tmin);// && tmin < tmax;
	}

	bool BoundingBox::Intersects(const BoundingSphere& sphere) const
	{
		return sphere.Intersects(*this);
	}

	bool BoundingBox::Intersects(const BoundingBox& box) const
	{
		_ASSERT(0); // not implemented
		return false;
	}

	//////////////////////////////////////////////////////

	BoundingFrustum::BoundingFrustum()
	{ }

	BoundingFrustum::BoundingFrustum(const Vector3& nearCenter, const Vector3& farCenter,
		const Vector2& nearExtent, const Vector2& farExtent)
	{
		Set(nearCenter, farCenter, nearExtent, farExtent);
	}

	BoundingFrustum::BoundingFrustum(
		const Vector3& nearCenter, const Vector3& farCenter,
		const Vector2& nearExtent, const Vector2& farExtent,
		const Vector3& forward, const Vector3& right, const Vector3& up)
	{
		Set(nearCenter, farCenter, nearExtent, farExtent, forward, right, up);
	}

	void BoundingFrustum::Set(
		const Vector3& nearCenter, const Vector3& farCenter,
		const Vector2& nearExtent, const Vector2& farExtent)
	{
		Vector3 forward = (farCenter - nearCenter).NormalizeFast();
		Vector3 right = (forward.Cross(Vector3(0, 1, 0))).NormalizeFast();
		Vector3 up = (forward.Cross(right)).NormalizeFast();

		Set(nearCenter, farCenter, nearExtent, farExtent, forward, right, up);
	}

	void BoundingFrustum::Set(
		const Vector3& nearCenter, const Vector3& farCenter,
		const Vector2& nearExtent, const Vector2& farExtent,
		const Vector3& forward, const Vector3& right, const Vector3& up)
	{
		Vector3 ntl = nearCenter + up * nearExtent.y - right * nearExtent.x;
		Vector3 ntr = nearCenter + up * nearExtent.y + right * nearExtent.x;
		Vector3 nbl = nearCenter - up * nearExtent.y - right * nearExtent.x;
		Vector3 nbr = nearCenter - up * nearExtent.y + right * nearExtent.x;

		Vector3 ftl = farCenter + up * farExtent.y - right * farExtent.x;
		Vector3 ftr = farCenter + up * farExtent.y + right * farExtent.x;
		Vector3 fbl = farCenter - up * farExtent.y - right * farExtent.x;
		Vector3 fbr = farCenter - up * farExtent.y + right * farExtent.x;

		// TOP
		mPlane[0].mPosition = ntl;
		mPlane[0].mNormal = (ftl - ntl).Cross(ntr - ntl).NormalizeFast();

		// BOTTOM
		mPlane[1].mPosition = nbr;
		mPlane[1].mNormal = (fbr - nbr).Cross(nbl - nbr).NormalizeFast();

		// LEFT
		mPlane[2].mPosition = nbl;
		mPlane[2].mNormal = (fbl - nbl).Cross(ntl - nbl).NormalizeFast();

		// RIGHT
		mPlane[3].mPosition = ntr;
		mPlane[3].mNormal = (fbr - ntr).Cross(nbr - ntr).NormalizeFast();

		// NEAR
		mPlane[4].mPosition = ntr;
		mPlane[4].mNormal = forward;

		// FAR
		mPlane[5].mPosition = ftl;
		mPlane[5].mNormal = -forward;
	}

	bool BoundingFrustum::Intersects(const Vector3& point) const
	{
		_ASSERT(0); // not implemented
		return false;
	}

	bool BoundingFrustum::Intersects(const Ray& ray, Vector3* intersection) const
	{
		_ASSERT(0); // not implemented
		return false;
	}

	bool BoundingFrustum::Intersects(const BoundingSphere& sphere) const
	{
		_ASSERT(0); // not implemented
		return false;
	}

	bool BoundingFrustum::Intersects(const BoundingBox& box) const
	{
		const Vector3& vmin = box.GetMinBounds();
		const Vector3& vmax = box.GetMaxBounds();

		for (int i = 0; i < 6; ++i)
		{
			Vector3 pv(vmax);	// positive vertex

			const Vector3& normal = mPlane[i].mNormal;

			// X axis
			if (normal.x < 0)
				pv.x = vmin.x;

			// Y axis 
			if (normal.y < 0)
				pv.y = vmin.y;

			// Z axis 
			if (normal.z < 0)
				pv.z = vmin.z;

			if (mPlane[i].Distance(pv) < 0)
				return false;
		}

		return true;
	}
}
