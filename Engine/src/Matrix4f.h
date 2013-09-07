#pragma once

#include "Common.h"

namespace Sentinel
{
	// Row-Major.
	//
	class SENTINEL_DLL Matrix4f
	{
		friend class Quatf;
		friend class Vector3f;
		friend class Vector4f;

	private:

		float		m[16];

	public:

		Matrix4f();
		Matrix4f( float* arr );
		Matrix4f( const Quatf& q );
		
		float		operator [] ( int i ) const;
		float&		operator [] ( int i );

		float*		Ptr();

		Matrix4f	operator + ( const Matrix4f& mat ) const;
		Matrix4f	Add( const Matrix4f& mat ) const;

		Matrix4f	operator * ( const Matrix4f& mat ) const;
		Matrix4f	Mul( const Matrix4f& mat ) const;
		
		void		Zero();
		void		Identity();
		
		void		Translate( const Vector3f& v );
		
		void		Rotate( const Quatf& q );
		void		Rotate( const Vector3f& v );
		void		RotateX( float degrees );
		void		RotateY( float degrees );
		void		RotateZ( float degrees );
		
		void		Scale( float _scale );
		void		Scale( const Vector3f& _scale );
		
		void		LookAtView( const Vector3f& pos, const Vector3f& lookAt, const Vector3f& up );
		void		BillboardAxis( const Vector3f& posBB, const Vector3f& posCamera, const Vector3f& up );
		void		BillboardWorld( const Vector3f& posBB, const Vector3f& posCamera, const Vector3f& up );
		
	private:

		void		Billboard( const Vector3f& i, const Vector3f& j, const Vector3f& k, const Vector3f& pos, bool transpose );

	public:

		float		Det();
		void		Transpose();
		void		InverseTrans();
		Matrix4f	InverseView();
		Matrix4f	Inverse() const;

		void		ProjectionOrthographic( float windowWidth, float windowHeight );
		void		ProjectionPerspective( float windowWidth, float windowHeight, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f );

		Vector3f	Transform( const Vector3f& v ) const;
		Vector3f	Transform( const Vector4f& v ) const;

		Vector3f	Forward();
		Vector3f	Up();
		Vector3f	Right();
	};
}
