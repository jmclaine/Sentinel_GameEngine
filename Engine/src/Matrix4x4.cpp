#include "MathUtil.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

namespace Sentinel
{
	const Matrix4x4 Matrix4x4::IDENTITY = Matrix4x4();

	Matrix4x4::Matrix4x4()
	{
		Identity();
	}

	Matrix4x4::Matrix4x4(float* arr)
	{
		for (int x = 0; x < 16; ++x)
			m[x] = arr[x];
	}

	Matrix4x4::Matrix4x4(const Quaternion& q)
	{
		Rotate(q);
	}

	float Matrix4x4::operator [] (int i) const
	{
		return m[i];
	}

	float& Matrix4x4::operator [] (int i)
	{
		return m[i];
	}

	float* Matrix4x4::Ptr()
	{
		return static_cast<float*>(m);
	}

	Matrix4x4 Matrix4x4::operator + (const Matrix4x4& mat) const
	{
		return Add(mat);
	}

	Matrix4x4 Matrix4x4::Add(const Matrix4x4& mat) const
	{
		Matrix4x4 result;

		for (int x = 0; x < 16; ++x)
		{
			result.m[x] = m[x] + mat.m[x];
		}

		return result;
	}

	Matrix4x4 Matrix4x4::operator * (const Matrix4x4& mat) const
	{
		return Mul(mat);
	}

	Matrix4x4 Matrix4x4::Mul(const Matrix4x4& mat) const
	{
		Matrix4x4 result;

		result.m[0] = m[0] * mat.m[0] + m[4] * mat.m[1] + m[8] * mat.m[2] + m[12] * mat.m[3];
		result.m[1] = m[1] * mat.m[0] + m[5] * mat.m[1] + m[9] * mat.m[2] + m[13] * mat.m[3];
		result.m[2] = m[2] * mat.m[0] + m[6] * mat.m[1] + m[10] * mat.m[2] + m[14] * mat.m[3];
		result.m[3] = m[3] * mat.m[0] + m[7] * mat.m[1] + m[11] * mat.m[2] + m[15] * mat.m[3];

		result.m[4] = m[0] * mat.m[4] + m[4] * mat.m[5] + m[8] * mat.m[6] + m[12] * mat.m[7];
		result.m[5] = m[1] * mat.m[4] + m[5] * mat.m[5] + m[9] * mat.m[6] + m[13] * mat.m[7];
		result.m[6] = m[2] * mat.m[4] + m[6] * mat.m[5] + m[10] * mat.m[6] + m[14] * mat.m[7];
		result.m[7] = m[3] * mat.m[4] + m[7] * mat.m[5] + m[11] * mat.m[6] + m[15] * mat.m[7];

		result.m[8] = m[0] * mat.m[8] + m[4] * mat.m[9] + m[8] * mat.m[10] + m[12] * mat.m[11];
		result.m[9] = m[1] * mat.m[8] + m[5] * mat.m[9] + m[9] * mat.m[10] + m[13] * mat.m[11];
		result.m[10] = m[2] * mat.m[8] + m[6] * mat.m[9] + m[10] * mat.m[10] + m[14] * mat.m[11];
		result.m[11] = m[3] * mat.m[8] + m[7] * mat.m[9] + m[11] * mat.m[10] + m[15] * mat.m[11];

		result.m[12] = m[0] * mat.m[12] + m[4] * mat.m[13] + m[8] * mat.m[14] + m[12] * mat.m[15];
		result.m[13] = m[1] * mat.m[12] + m[5] * mat.m[13] + m[9] * mat.m[14] + m[13] * mat.m[15];
		result.m[14] = m[2] * mat.m[12] + m[6] * mat.m[13] + m[10] * mat.m[14] + m[14] * mat.m[15];
		result.m[15] = m[3] * mat.m[12] + m[7] * mat.m[13] + m[11] * mat.m[14] + m[15] * mat.m[15];

		return result;
	}

	void Matrix4x4::Zero()
	{
		memset(m, 0, sizeof(float) * 16);
	}

	void Matrix4x4::Identity()
	{
		Zero();

		m[0] = 1.0f;
		m[5] = 1.0f;
		m[10] = 1.0f;
		m[15] = 1.0f;
	};

	void Matrix4x4::Translate(const Vector3& v)
	{
		Identity();

		m[12] = v.x;
		m[13] = v.y;
		m[14] = v.z;
	}

	void Matrix4x4::Rotate(const Quaternion& q)
	{
		Zero();

		float x_x = q.x * q.x;
		float y_y = q.y * q.y;
		float z_z = q.z * q.z;
		float w_w = q.w * q.w;

		//float inv = 1.0f / (x_x + y_y + z_z + w_w);

		m[0] = (x_x - y_y - z_z + w_w);
		m[5] = (-x_x + y_y - z_z + w_w);
		m[10] = (-x_x - y_y + z_z + w_w);

		float tmp1 = q.x * q.y;
		float tmp2 = q.z * q.w;
		m[1] = 2.0f * (tmp1 + tmp2);
		m[4] = 2.0f * (tmp1 - tmp2);

		tmp1 = q.x * q.z;
		tmp2 = q.y * q.w;
		m[2] = 2.0f * (tmp1 - tmp2);
		m[8] = 2.0f * (tmp1 + tmp2);

		tmp1 = q.y * q.z;
		tmp2 = q.x * q.w;
		m[6] = 2.0f * (tmp1 + tmp2);
		m[9] = 2.0f * (tmp1 - tmp2);

		m[15] = 1.0f;
	}

	void Matrix4x4::Rotate(const Vector3& v)
	{
		Rotate(Quaternion(0, 1, 0, v.y).AxisAngle() * Quaternion(1, 0, 0, v.x).AxisAngle() * Quaternion(0, 0, 1, v.z).AxisAngle());
	}

	void Matrix4x4::RotateX(float degrees)
	{
		Rotate(Quaternion(1, 0, 0, degrees).AxisAngle());

		/*
		degrees *= (Real)DEGREES_TO_RADIANS;

		Zero();
		m[0]  = 1;
		m[5]  = cos( degrees );
		m[6]  = sin( degrees );

		m[9]  = -sin( degrees );
		m[10] = cos( degrees );

		m[15] = 1;
		*/
	}

	void Matrix4x4::RotateY(float degrees)
	{
		Rotate(Quaternion(0, 1, 0, degrees).AxisAngle());

		/*
		degrees *= (Real)DEGREES_TO_RADIANS;

		Zero();
		m[0]  = cos( degrees );
		m[2]  = -sin( degrees );
		m[5]  = 1;

		m[8]  = sin( degrees );
		m[10] = cos( degrees );

		m[15] = 1;
		*/
	}

	void Matrix4x4::RotateZ(float degrees)
	{
		Rotate(Quaternion(0, 0, 1, degrees).AxisAngle());

		/*
		degrees *= (Real)DEGREES_TO_RADIANS;

		Zero();
		m[0]  = cos( degrees );
		m[1]  = sin( degrees );

		m[4]  = -sin( degrees );
		m[5]  = cos( degrees );

		m[10] = 1;
		m[15] = 1;
		*/
	}

	void Matrix4x4::Scale(float _scale)
	{
		Scale(Vector3(_scale, _scale, _scale));
	}

	void Matrix4x4::Scale(const Vector3& _scale)
	{
		Zero();

		m[0] = _scale.x;
		m[5] = _scale.y;
		m[10] = _scale.z;
		m[15] = 1.0f;
	}

	void Matrix4x4::Billboard(const Vector3& i, const Vector3& j, const Vector3& k, const Vector3& pos, bool transpose)
	{
		m[0] = i.x;
		m[1] = i.y;
		m[2] = i.z;
		m[3] = 0.0f;

		m[4] = j.x;
		m[5] = j.y;
		m[6] = j.z;
		m[7] = 0.0f;

		m[8] = k.x;
		m[9] = k.y;
		m[10] = k.z;
		m[11] = 0.0f;

		m[12] = 0.0f;
		m[13] = 0.0f;
		m[14] = 0.0f;
		m[15] = 1.0f;

		if (transpose)
			Transpose();
	}

	void Matrix4x4::LookAtView(const Vector3& pos, const Vector3& lookAt, const Vector3& up)
	{
		Vector3 i, j, k;

		// Forward
		k = (pos - lookAt).Normalize();

		// Side
		j = up;
		i = j.Cross(k).Normalize();

		// Up
		j = k.Cross(i);

		Matrix4x4 matTrans;
		matTrans.Translate(-pos);

		Billboard(i, j, k, -pos, true);

		*this = *this * matTrans;
	}

	void Matrix4x4::BillboardAxis(const Vector3& posBB, const Vector3& posCamera, const Vector3& up)
	{
		Vector3 i, j, k;

		j = up.Normalize();

		i = j.Cross(posCamera - posBB).Normalize();

		k = i.Cross(j).Normalize();

		Billboard(i, j, k, posBB, false);
	}

	void Matrix4x4::BillboardWorld(const Vector3& posBB, const Vector3& posCamera, const Vector3& up)
	{
		Vector3 i, j, k;

		// Forward
		k = (posCamera - posBB).Normalize();

		// Side
		j = up.Normalize();
		i = j.Cross(k).Normalize();

		// Up
		j = k.Cross(i).Normalize();

		Billboard(i, j, k, posBB, false);
	}

	void Matrix4x4::ProjectionOrthographic(float windowWidth, float windowHeight)
	{
		Zero();

		m[0] = 2.0f / windowWidth;
		m[5] = -2.0f / windowHeight;
		m[10] = 1.0f;

		m[12] = -1.0f;
		m[13] = 1.0f;
		m[14] = 0.0f;
		m[15] = 1.0f;
	}

	void Matrix4x4::ProjectionPerspective(float windowWidth, float windowHeight, float nearZ, float farZ, float FOV)
	{
		Zero();

		float size = 1.0f / tan((float)DEGREES_TO_RADIANS * FOV * 0.5f);
		float depth = 1.0f / (nearZ - farZ);

		m[0] = size / (windowWidth / windowHeight);
		m[5] = size;
		m[10] = (farZ + nearZ) * depth;
		m[11] = -1.0f;
		m[14] = 2.0f * (nearZ * farZ) * depth;
		m[15] = 0.0f;
	}

	void Matrix4x4::World(const Vector3& position, const Quaternion& orientation, const Vector3& scale)
	{
		// TODO: Do all matrix math without using matrices.
		//
		static Matrix4x4 TRANSLATION;
		static Matrix4x4 ROTATION;
		static Matrix4x4 SCALE;

		TRANSLATION.Translate(position);
		ROTATION.Rotate(orientation);
		SCALE.Scale(scale);

		*this = TRANSLATION * ROTATION * SCALE;
	}

	void Matrix4x4::World(const Vector3& position, const Quaternion& orientation, const Vector3& scale, const Vector3& offset)
	{
		// TODO: Do all matrix math without using matrices.
		//
		static Matrix4x4 TRANSLATION;
		static Matrix4x4 ROTATION;
		static Matrix4x4 SCALE;
		static Matrix4x4 OFFSET;

		TRANSLATION.Translate(position);
		ROTATION.Rotate(orientation);
		SCALE.Scale(scale);
		OFFSET.Translate(offset);

		*this = TRANSLATION * ROTATION * OFFSET * SCALE;
	}

	void Matrix4x4::World(const Vector2& position, float degrees, const Vector2& scale)
	{
		// TODO: Do all matrix math without using matrices.
		//
		static Matrix4x4 TRANSLATION;
		static Matrix4x4 ROTATION;
		static Matrix4x4 SCALE;

		TRANSLATION.Translate(Vector3(position.x, position.y, 0));
		ROTATION.Rotate(Quaternion(0, 0, 1, degrees).AxisAngle());
		SCALE.Scale(Vector3(scale.x, scale.y, 1));

		*this = TRANSLATION * ROTATION * SCALE;
	}

	float Matrix4x4::Det()
	{
		float left1 = m[0] * m[5] - m[4] * m[1];
		float left2 = m[0] * m[9] - m[8] * m[1];
		float left3 = m[0] * m[13] - m[12] * m[1];
		float left4 = m[0] * m[9] - m[8] * m[5];
		float left5 = m[4] * m[13] - m[12] * m[5];
		float left6 = m[8] * m[13] - m[12] * m[9];

		float right1 = m[2] * m[7] - m[6] * m[3];
		float right2 = m[2] * m[11] - m[10] * m[3];
		float right3 = m[2] * m[15] - m[14] * m[3];
		float right4 = m[6] * m[11] - m[10] * m[7];
		float right5 = m[6] * m[15] - m[14] * m[7];
		float right6 = m[10] * m[15] - m[14] * m[11];

		return (
			left1 * right6 - left2 * right5 + \
			left3 * right4 + left4 * right3 - \
			left5 * right2 + left6 * right1);
	}

	void Matrix4x4::Transpose()
	{
		std::swap(m[1], m[4]);
		std::swap(m[2], m[8]);
		std::swap(m[3], m[12]);
		std::swap(m[6], m[9]);
		std::swap(m[7], m[13]);
		std::swap(m[11], m[14]);
	}

	void Matrix4x4::InverseTrans()
	{
		m[12] *= -1;
		m[13] *= -1;
		m[14] *= -1;
	}

	Matrix4x4 Matrix4x4::InverseView()
	{
		Vector3 v(m[12], m[13], m[14]);

		Vector3 w(
			v.Dot(Vector3(m[0], m[1], m[2])),
			v.Dot(Vector3(m[4], m[5], m[6])),
			v.Dot(Vector3(m[8], m[9], m[10])));

		Matrix4x4 temp;

		temp.m[0] = m[0];		temp.m[1] = m[4];		temp.m[2] = m[8];		temp.m[3] = 0;
		temp.m[4] = m[1];		temp.m[5] = m[5];		temp.m[6] = m[9];		temp.m[7] = 0;
		temp.m[8] = m[2];		temp.m[9] = m[6];		temp.m[10] = m[10];		temp.m[11] = 0;
		temp.m[12] = -w.x;		temp.m[13] = -w.y;		temp.m[14] = -w.z;		temp.m[15] = 1;

		return temp;
	}

	Matrix4x4 Matrix4x4::Inverse() const
	{
		Matrix4x4 inv;

		float m10_15 = m[10] * m[15];	float m11_14 = m[11] * m[14];	float m4_1 = m[4] * m[1];
		float m4_9 = m[4] * m[9];		float m11_13 = m[11] * m[13];	float m2_15 = m[2] * m[15];
		float m0_9 = m[0] * m[9];		float m10_13 = m[10] * m[13];	float m8_5 = m[8] * m[5];
		float m6_15 = m[6] * m[15];		float m0_7 = m[0] * m[7];		float m8_1 = m[8] * m[1];
		float m0_5 = m[0] * m[5];		float m1_7 = m[1] * m[7];		float m2_11 = m[2] * m[11];
		float m6_11 = m[6] * m[11];		float m0_6 = m[0] * m[6];		float m8_13 = m[8] * m[13];
		float m8_3 = m[8] * m[3];

		float m4_3 = m[4] * m[3];		float m12_5 = m[12] * m[5];		float m7_10 = m[7] * m[10];
		float m9_14 = m[9] * m[14];		float m12_1 = m[12] * m[1];		float m12_9 = m[12] * m[9];
		float m5_3 = m[5] * m[3];		float m2_7 = m[2] * m[7];		float m3_10 = m[3] * m[10];
		float m4_2 = m[4] * m[2];										float m3_6 = m[3] * m[6];
		float m8_7 = m[8] * m[7];

		inv.m[0] = m[5] * m10_15 - m[5] * m11_14 - m[9] * m6_15 + m9_14*m[7] + m[13] * m6_11 - m[13] * m7_10;
		inv.m[4] = -m[4] * m10_15 + m[4] * m11_14 + m[8] * m6_15 - m8_7*m[14] - m[12] * m6_11 + m[12] * m7_10;
		inv.m[8] = m4_9*m[15] - m[4] * m11_13 - m8_5*m[15] + m8_7*m[13] + m12_5*m[11] - m12_9*m[7];
		inv.m[12] = -m4_9*m[14] + m[4] * m10_13 + m8_5*m[14] - m8_13*m[6] - m12_5*m[10] + m12_9*m[6];
		inv.m[1] = -m[1] * m10_15 + m[1] * m11_14 + m[9] * m2_15 - m9_14*m[3] - m[13] * m2_11 + m[13] * m3_10;
		inv.m[5] = m[0] * m10_15 - m[0] * m11_14 - m[8] * m2_15 + m8_3*m[14] + m[12] * m2_11 - m[12] * m3_10;
		inv.m[9] = -m0_9*m[15] + m[0] * m11_13 + m8_1*m[15] - m8_13*m[3] - m12_1*m[11] + m12_9*m[3];
		inv.m[13] = m0_9*m[14] - m[0] * m10_13 - m8_1*m[14] + m8_13*m[2] + m12_1*m[10] - m12_9*m[2];
		inv.m[2] = m[1] * m6_15 - m1_7*m[14] - m[5] * m2_15 + m5_3*m[14] + m[13] * m2_7 - m[13] * m3_6;
		inv.m[6] = -m[0] * m6_15 + m0_7*m[14] + m[4] * m2_15 - m4_3*m[14] - m[12] * m2_7 + m[12] * m3_6;
		inv.m[10] = m0_5*m[15] - m0_7*m[13] - m4_1*m[15] + m4_3*m[13] + m12_1*m[7] - m12_5*m[3];
		inv.m[14] = -m0_5*m[14] + m0_6*m[13] + m4_1*m[14] - m4_2*m[13] - m12_1*m[6] + m12_5*m[2];
		inv.m[3] = -m[1] * m6_11 + m1_7*m[10] + m[5] * m2_11 - m5_3*m[10] - m[9] * m2_7 + m[9] * m3_6;
		inv.m[7] = m[0] * m6_11 - m0_7*m[10] - m[4] * m2_11 + m4_3*m[10] + m[8] * m2_7 - m[8] * m3_6;
		inv.m[11] = -m0_5*m[11] + m0_7*m[9] + m4_1*m[11] - m4_3*m[9] - m8_1*m[7] + m8_5*m[3];
		inv.m[15] = m0_5*m[10] - m0_6*m[9] - m4_1*m[10] + m4_2*m[9] + m8_1*m[6] - m8_5*m[2];

		float det = m[0] * inv.m[0] + m[1] * inv.m[4] + m[2] * inv.m[8] + m[3] * inv.m[12];

		if (det == 0)
		{
			inv.Identity();
			return inv;
		}

		det = 1.0f / det;

		for (int x = 0; x < 16; x++)
		{
			inv.m[x] *= det;
		}

		return inv;
	}

	Vector3 Matrix4x4::Transform(const Vector3& v, float w) const
	{
		return Vector3(
			v.x*m[0] + v.y*m[4] + v.z*m[8] + w*m[12],
			v.x*m[1] + v.y*m[5] + v.z*m[9] + w*m[13],
			v.x*m[2] + v.y*m[6] + v.z*m[10] + w*m[14]);
	}

	Vector3 Matrix4x4::Transform(const Vector4& v) const
	{
		return Vector3(
			v.x*m[0] + v.y*m[4] + v.z*m[8] + v.w*m[12],
			v.x*m[1] + v.y*m[5] + v.z*m[9] + v.w*m[13],
			v.x*m[2] + v.y*m[6] + v.z*m[10] + v.w*m[14]);
	}

	Vector3 Matrix4x4::Right() const
	{
		return Vector3(m[0], m[1], m[2]).NormalizeFast();
	}

	Vector3 Matrix4x4::Up() const
	{
		return Vector3(m[4], m[5], m[6]).NormalizeFast();
	}

	Vector3 Matrix4x4::Forward() const
	{
		return Vector3(-m[8], -m[9], -m[10]).NormalizeFast();
	}
}
