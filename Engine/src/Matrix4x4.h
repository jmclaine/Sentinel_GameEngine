#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	// Row-Major.
	//
	class SENTINEL_DLL Matrix4x4
	{
		friend class Quaternion;
		friend class Vector2;
		friend class Vector3;
		friend class Vector4;

	private:

		float m[16];

	public:

		static const Matrix4x4 IDENTITY;

		////////////////////////////////

		Matrix4x4();
		Matrix4x4(float* arr);
		Matrix4x4(const Quaternion& q);

		float operator [] (int i) const;
		float& operator [] (int i);

		float* Ptr();

		Matrix4x4 operator + (const Matrix4x4& mat) const;
		Matrix4x4 Add(const Matrix4x4& mat) const;

		Matrix4x4 operator * (const Matrix4x4& mat) const;
		Matrix4x4 Mul(const Matrix4x4& mat) const;

		void Zero();
		void Identity();

		void Translate(const Vector3& v);

		void Rotate(const Quaternion& q);
		void Rotate(const Vector3& v);
		void RotateX(float degrees);
		void RotateY(float degrees);
		void RotateZ(float degrees);

		void Scale(float _scale);
		void Scale(const Vector3& _scale);

		void LookAtView(const Vector3& pos, const Vector3& lookAt, const Vector3& up);
		void BillboardAxis(const Vector3& posBB, const Vector3& posCamera, const Vector3& up);
		void BillboardWorld(const Vector3& posBB, const Vector3& posCamera, const Vector3& up);

	private:

		void Billboard(const Vector3& i, const Vector3& j, const Vector3& k, const Vector3& pos, bool transpose);

	public:

		float Det();
		void Transpose();
		void InverseTrans();
		Matrix4x4 InverseView();
		Matrix4x4 Inverse() const;

		void ProjectionOrthographic(float windowWidth, float windowHeight);
		void ProjectionPerspective(float windowWidth, float windowHeight, float nearZ = 0.3f, float farZ = 1000.0f, float FOV = 60.0f);

		void World(const Vector3& position, const Quaternion& orientation, const Vector3& scale);
		void World(const Vector3& position, const Quaternion& orientation, const Vector3& scale, const Vector3& offset);
		void World(const Vector2& position, float degrees, const Vector2& scale);

		Vector3 Transform(const Vector3& v, float w = 1.0f) const;
		Vector3 Transform(const Vector4& v) const;

		Vector3 Forward() const;
		Vector3 Up() const;
		Vector3 Right() const;
	};
}
