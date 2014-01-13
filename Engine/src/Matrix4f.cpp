#include "MathCommon.h"
#include "Matrix4f.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Quatf.h"

namespace Sentinel
{
	const Matrix4f Matrix4f::IDENTITY = Matrix4f();

	Matrix4f::Matrix4f()
	{
		Identity();
	}

	Matrix4f::Matrix4f( float* arr )
	{
		for( int x = 0; x < 16; ++x )
			m[ x ] = arr[ x ];
	}

	Matrix4f::Matrix4f( const Quatf& q )
	{
		Rotate( q );
	}

	float Matrix4f::operator [] ( int i ) const
	{
		return m[ i ];
	}

	float& Matrix4f::operator [] ( int i )
	{
		return m[ i ];
	}

	float* Matrix4f::Ptr()
	{
		return static_cast< float* >(m);
	}

	Matrix4f Matrix4f::operator + ( const Matrix4f& mat ) const
	{
		return Add( mat );
	}

	Matrix4f Matrix4f::Add( const Matrix4f& mat ) const
	{
		Matrix4f result;

		for( int x = 0; x < 16; ++x )
		{
			result.m[ x ] = m[ x ] + mat.m[ x ];
		}

		return result;
	}

	Matrix4f Matrix4f::operator * ( const Matrix4f& mat ) const
	{
		return Mul( mat );
	}

	Matrix4f Matrix4f::Mul( const Matrix4f& mat ) const
	{
		Matrix4f result;

		result.m[0]  = m[0] * mat.m[0]  + m[4] * mat.m[1]  + m[8]  * mat.m[2]  + m[12] * mat.m[3];
		result.m[1]  = m[1] * mat.m[0]  + m[5] * mat.m[1]  + m[9]  * mat.m[2]  + m[13] * mat.m[3];
		result.m[2]  = m[2] * mat.m[0]  + m[6] * mat.m[1]  + m[10] * mat.m[2]  + m[14] * mat.m[3];
		result.m[3]  = m[3] * mat.m[0]  + m[7] * mat.m[1]  + m[11] * mat.m[2]  + m[15] * mat.m[3];

		result.m[4]  = m[0] * mat.m[4]  + m[4] * mat.m[5]  + m[8]  * mat.m[6]  + m[12] * mat.m[7];
		result.m[5]  = m[1] * mat.m[4]  + m[5] * mat.m[5]  + m[9]  * mat.m[6]  + m[13] * mat.m[7];
		result.m[6]  = m[2] * mat.m[4]  + m[6] * mat.m[5]  + m[10] * mat.m[6]  + m[14] * mat.m[7];
		result.m[7]  = m[3] * mat.m[4]  + m[7] * mat.m[5]  + m[11] * mat.m[6]  + m[15] * mat.m[7];

		result.m[8]  = m[0] * mat.m[8]  + m[4] * mat.m[9]  + m[8]  * mat.m[10] + m[12] * mat.m[11];
		result.m[9]  = m[1] * mat.m[8]  + m[5] * mat.m[9]  + m[9]  * mat.m[10] + m[13] * mat.m[11];
		result.m[10] = m[2] * mat.m[8]  + m[6] * mat.m[9]  + m[10] * mat.m[10] + m[14] * mat.m[11];
		result.m[11] = m[3] * mat.m[8]  + m[7] * mat.m[9]  + m[11] * mat.m[10] + m[15] * mat.m[11];

		result.m[12] = m[0] * mat.m[12] + m[4] * mat.m[13] + m[8]  * mat.m[14] + m[12] * mat.m[15];
		result.m[13] = m[1] * mat.m[12] + m[5] * mat.m[13] + m[9]  * mat.m[14] + m[13] * mat.m[15];
		result.m[14] = m[2] * mat.m[12] + m[6] * mat.m[13] + m[10] * mat.m[14] + m[14] * mat.m[15];
		result.m[15] = m[3] * mat.m[12] + m[7] * mat.m[13] + m[11] * mat.m[14] + m[15] * mat.m[15];

		return result;
	}

	void Matrix4f::Zero()
	{
		memset( m, 0, sizeof(float)*16 );
	}

	void Matrix4f::Identity()
	{
		Zero();

		m[0]  = 1.0f;
		m[5]  = 1.0f;
		m[10] = 1.0f;
		m[15] = 1.0f;
	};

	void Matrix4f::Translate( const Vector3f& v )
	{
		Identity();

		m[12] = v.x;
		m[13] = v.y;
		m[14] = v.z;
	}

	void Matrix4f::Rotate( const Quatf& q )
	{
		Zero();

		float x_x = q.x * q.x;
		float y_y = q.y * q.y;
		float z_z = q.z * q.z;
		float w_w = q.w * q.w;
		
		//float inv = 1.0f / (x_x + y_y + z_z + w_w);
 
		m[0]  = ( x_x - y_y - z_z + w_w);
		m[5]  = (-x_x + y_y - z_z + w_w);
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

	void Matrix4f::Rotate( const Vector3f& v )
	{
		Rotate( Quatf( 0, 1, 0, v.y ).AxisAngle() * Quatf( 1, 0, 0, v.x ).AxisAngle() * Quatf( 0, 0, 1, v.z ).AxisAngle() );
	}

	void Matrix4f::RotateX( float degrees )
	{
		Rotate( Quatf( 1, 0, 0, degrees ).AxisAngle() );

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

	void Matrix4f::RotateY( float degrees )
	{
		Rotate( Quatf( 0, 1, 0, degrees ).AxisAngle() );

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

	void Matrix4f::RotateZ( float degrees )
	{
		Rotate( Quatf( 0, 0, 1, degrees ).AxisAngle() );

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

	void Matrix4f::Scale( float _scale )
	{
		Scale( Vector3f( _scale, _scale, _scale ));
	}

	void Matrix4f::Scale( const Vector3f& _scale )
	{
		Zero();

		m[0]  = _scale.x;
		m[5]  = _scale.y;
		m[10] = _scale.z;
		m[15] = 1.0f;
	}

	void Matrix4f::Billboard( const Vector3f& i, const Vector3f& j, const Vector3f& k, const Vector3f& pos, bool transpose )
	{
		Matrix4f matLookAt;
		
		matLookAt.m[0]  = i.x;
		matLookAt.m[1]  = i.y;
		matLookAt.m[2]  = i.z;
		matLookAt.m[3]  = 0.0f;

		matLookAt.m[4]  = j.x;
		matLookAt.m[5]  = j.y;
		matLookAt.m[6]  = j.z;
		matLookAt.m[7]  = 0.0f;

		matLookAt.m[8]  = k.x;
		matLookAt.m[9]  = k.y;
		matLookAt.m[10] = k.z;
		matLookAt.m[11] = 0.0f;

		matLookAt.m[12] = 0.0f;
		matLookAt.m[13] = 0.0f;
		matLookAt.m[14] = 0.0f;
		matLookAt.m[15] = 1.0f;

		if( transpose )
		{
			matLookAt.Transpose();
		}

		Matrix4f matTrans;
		matTrans.Translate( pos );

		*this = matLookAt * matTrans;
	}

	void Matrix4f::LookAtView( const Vector3f& pos, const Vector3f& lookAt, const Vector3f& up )
	{
		Vector3f i, j, k;

		// Forward
		k = Vector3f( pos.x-lookAt.x, pos.y-lookAt.y, pos.z-lookAt.z ).Normalize();

		// Side
		j = up;
		i = j.Cross( k ).Normalize();

		// Up
		j = k.Cross( i );

		Billboard( i, j, k, pos * -1.0f, true );
	}

	void Matrix4f::BillboardAxis( const Vector3f& posBB, const Vector3f& posCamera, const Vector3f& up )
	{
		Vector3f i, j, k;

		j = up.Normalize();

		i = j.Cross( posCamera.Sub( posBB )).Normalize();

		k = i.Cross( j ).Normalize();

		Billboard( i, j, k, posBB, false );
	}

	void Matrix4f::BillboardWorld( const Vector3f& posBB, const Vector3f& posCamera, const Vector3f& up )
	{
		Vector3f i, j, k;

		// Forward
		k = posCamera.Sub( posBB ).Normalize();

		// Side
		j = up.Normalize();
		i = j.Cross( k ).Normalize();

		// Up
		j = k.Cross( i ).Normalize();

		Billboard( i, j, k, posBB, false );
	}

	void Matrix4f::ProjectionOrthographic( float windowWidth, float windowHeight )
	{
		Zero();

		m[0]  =  2.0f / windowWidth;
		m[5]  = -2.0f / windowHeight;
		m[10] =  1.0f;

		m[12] = -1.0f;
		m[13] =  1.0f;
		m[14] =  0.0f;
		m[15] =  1.0f;
	}

	void Matrix4f::ProjectionPerspective( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV )
	{
		Zero();

		float size  = 1.0f / tan( (float)DEGREES_TO_RADIANS * FOV * 0.5f );
		float depth = 1.0f / (nearZ - farZ);

		m[0]  = size / (windowWidth / windowHeight);
		m[5]  = size;
		m[10] = (farZ + nearZ) * depth;
		m[11] = -1.0f;
		m[14] = 2.0f * (nearZ * farZ) * depth;
		m[15] = 0.0f;
	}

	void Matrix4f::World( const Vector3f& position, const Quatf& orientation, const Vector3f& scale )
	{
		// TODO: Do all matrix math without using matrices.
		//
		static Matrix4f TRANSLATION;
		static Matrix4f ROTATION;
		static Matrix4f SCALE;

		TRANSLATION.Translate( position );
		ROTATION.Rotate( orientation );
		SCALE.Scale( scale );

		*this = TRANSLATION * ROTATION * SCALE;
	}

	void Matrix4f::World( const Vector2f& position, float degrees, const Vector2f& scale )
	{
		// TODO: Do all matrix math without using matrices.
		//
		static Matrix4f TRANSLATION;
		static Matrix4f ROTATION;
		static Matrix4f SCALE;

		TRANSLATION.Translate( Vector3f( position.x, position.y, 0 ));
		ROTATION.Rotate( Quatf( 0, 0, 1, degrees ).AxisAngle() );
		SCALE.Scale( Vector3f( scale.x, scale.y, 1 ));

		*this = TRANSLATION * ROTATION * SCALE;
	}

	float Matrix4f::Det()
	{
		float left1 = m[0] * m[5]  - m[4]  * m[1];
		float left2 = m[0] * m[9]  - m[8]  * m[1];
		float left3 = m[0] * m[13] - m[12] * m[1];
		float left4 = m[0] * m[9]  - m[8]  * m[5];
		float left5 = m[4] * m[13] - m[12] * m[5];
		float left6 = m[8] * m[13] - m[12] * m[9];

		float right1 = m[2]  * m[7]  - m[6]  * m[3];
		float right2 = m[2]  * m[11] - m[10] * m[3];
		float right3 = m[2]  * m[15] - m[14] * m[3];
		float right4 = m[6]  * m[11] - m[10] * m[7];
		float right5 = m[6]  * m[15] - m[14] * m[7];
		float right6 = m[10] * m[15] - m[14] * m[11];

		return (left1 * right6 - left2 * right5 + \
			    left3 * right4 + left4 * right3 - \
			    left5 * right2 + left6 * right1);
	}

	void Matrix4f::Transpose()
	{
		std::swap( m[1],  m[4]  );
		std::swap( m[2],  m[8]  );
		std::swap( m[3],  m[12] );
		std::swap( m[6],  m[9]  );
		std::swap( m[7],  m[13] );
		std::swap( m[11], m[14] );
	}

	void Matrix4f::InverseTrans()
	{
		m[12] *= -1;
		m[13] *= -1;
		m[14] *= -1;
	}

	Matrix4f Matrix4f::InverseView()
	{
		Vector3f v( m[12], m[13], m[14] );

		Vector3f w( v.Dot( Vector3f( m[0], m[1], m[2] )), 
					v.Dot( Vector3f( m[4], m[5], m[6] )), 
					v.Dot( Vector3f( m[8], m[9], m[10] )));

		Matrix4f temp;

		temp.m[0]  = m[0];		temp.m[1]  = m[4];		temp.m[2]  = m[8];		temp.m[3]  = 0;
		temp.m[4]  = m[1];		temp.m[5]  = m[5];		temp.m[6]  = m[9];		temp.m[7]  = 0;
		temp.m[8]  = m[2];		temp.m[9]  = m[6];		temp.m[10] = m[10];		temp.m[11] = 0;
		temp.m[12] = -w.x;		temp.m[13] = -w.y;		temp.m[14] = -w.z;		temp.m[15] = 1;

		return temp;
	}

	Matrix4f Matrix4f::Inverse() const
	{
		Matrix4f inv;

		float m10_15 = m[10]*m[15];	float m11_14 = m[11]*m[14];	float m4_1	= m[4]*m[1];
		float m4_9   = m[4]*m[9];	float m11_13 = m[11]*m[13];	float m2_15	= m[2]*m[15];
		float m0_9	 = m[0]*m[9];	float m10_13 = m[10]*m[13];	float m8_5	= m[8]*m[5];
		float m6_15	 = m[6]*m[15];	float m0_7	 = m[0]*m[7];	float m8_1	= m[8]*m[1];
		float m0_5	 = m[0]*m[5];	float m1_7	 = m[1]*m[7];	float m2_11	= m[2]*m[11];
		float m6_11	 = m[6]*m[11];	float m0_6	 = m[0]*m[6];	float m8_13	= m[8]*m[13];
																float m8_3	= m[8]*m[3];

		float m4_3	= m[4]*m[3];	float m12_5	= m[12]*m[5];	float m7_10	= m[7]*m[10];
		float m9_14	= m[9]*m[14];	float m12_1	= m[12]*m[1];	float m12_9	= m[12]*m[9];
		float m5_3	= m[5]*m[3];	float m2_7	= m[2]*m[7];	float m3_10	= m[3]*m[10];
		float m4_2	= m[4]*m[2];								float m3_6	= m[3]*m[6];
		float m8_7	= m[8]*m[7];

		inv.m[0]  =  m[5]*m10_15 - m[5]*m11_14 - m[9]*m6_15 + m9_14*m[7] + m[13]*m6_11 - m[13]*m7_10;
		inv.m[4]  = -m[4]*m10_15 + m[4]*m11_14 + m[8]*m6_15 - m8_7*m[14] - m[12]*m6_11 + m[12]*m7_10;
		inv.m[8]  =  m4_9*m[15]  - m[4]*m11_13 - m8_5*m[15] + m8_7*m[13] + m12_5*m[11] - m12_9*m[7];
		inv.m[12] = -m4_9*m[14]  + m[4]*m10_13 + m8_5*m[14] - m8_13*m[6] - m12_5*m[10] + m12_9*m[6];
		inv.m[1]  = -m[1]*m10_15 + m[1]*m11_14 + m[9]*m2_15 - m9_14*m[3] - m[13]*m2_11 + m[13]*m3_10;
		inv.m[5]  =  m[0]*m10_15 - m[0]*m11_14 - m[8]*m2_15 + m8_3*m[14] + m[12]*m2_11 - m[12]*m3_10;
		inv.m[9]  = -m0_9*m[15]  + m[0]*m11_13 + m8_1*m[15] - m8_13*m[3] - m12_1*m[11] + m12_9*m[3];
		inv.m[13] =  m0_9*m[14]  - m[0]*m10_13 - m8_1*m[14] + m8_13*m[2] + m12_1*m[10] - m12_9*m[2];
		inv.m[2]  =  m[1]*m6_15  - m1_7*m[14]  - m[5]*m2_15 + m5_3*m[14] + m[13]*m2_7  - m[13]*m3_6;
		inv.m[6]  = -m[0]*m6_15  + m0_7*m[14]  + m[4]*m2_15 - m4_3*m[14] - m[12]*m2_7  + m[12]*m3_6;
		inv.m[10] =  m0_5*m[15]  - m0_7*m[13]  - m4_1*m[15] + m4_3*m[13] + m12_1*m[7]  - m12_5*m[3];
		inv.m[14] = -m0_5*m[14]  + m0_6*m[13]  + m4_1*m[14] - m4_2*m[13] - m12_1*m[6]  + m12_5*m[2];
		inv.m[3]  = -m[1]*m6_11  + m1_7*m[10]  + m[5]*m2_11 - m5_3*m[10] - m[9]*m2_7   + m[9]*m3_6;
		inv.m[7]  =  m[0]*m6_11  - m0_7*m[10]  - m[4]*m2_11 + m4_3*m[10] + m[8]*m2_7   - m[8]*m3_6;
		inv.m[11] = -m0_5*m[11]  + m0_7*m[9]   + m4_1*m[11] - m4_3*m[9]  - m8_1*m[7]   + m8_5*m[3];
		inv.m[15] =  m0_5*m[10]  - m0_6*m[9]   - m4_1*m[10] + m4_2*m[9]  + m8_1*m[6]   - m8_5*m[2];

		float det = m[0]*inv.m[0] + m[1]*inv.m[4] + m[2]*inv.m[8] + m[3]*inv.m[12];

		if( det == 0 )
		{
			inv.Identity();
			return inv;
		}

		det = 1.0f / det;

		for( int x = 0; x < 16; x++ )
		{
			inv.m[x] *= det;
		}

		return inv;
	}

	Vector3f Matrix4f::Transform( const Vector3f& v ) const
	{
		// assume v.w = 1
		return Vector3f( v.x*m[0]+v.y*m[4]+v.z*m[8]+m[12], 
						 v.x*m[1]+v.y*m[5]+v.z*m[9]+m[13], 
						 v.x*m[2]+v.y*m[6]+v.z*m[10]+m[14] );
	}

	Vector3f Matrix4f::Transform( const Vector4f& v ) const
	{
		return Vector3f( v.x*m[0]+v.y*m[4]+v.z*m[8]+v.w*m[12], 
						 v.x*m[1]+v.y*m[5]+v.z*m[9]+v.w*m[13], 
						 v.x*m[2]+v.y*m[6]+v.z*m[10]+v.w*m[14] );
	}

	Vector3f Matrix4f::Right() const
	{
		return Vector3f( m[0], m[1], m[2] );
	}

	Vector3f Matrix4f::Up() const
	{
		return Vector3f( m[4], m[5], m[6] );
	}

	Vector3f Matrix4f::Forward() const
	{
		return Vector3f( -m[8], -m[9], -m[10] );
	}
}
