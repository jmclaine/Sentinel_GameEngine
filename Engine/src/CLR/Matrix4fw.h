#pragma once

#include "Matrix4f.h"
#include "Vector3fw.h"
#include "Quatfw.h"

using namespace System;

namespace Sentinel { namespace Math
{
	public ref class Matrix4fw
	{
	private:
		
		Matrix4f*	mRef;

	public:

		Matrix4fw();
		Matrix4fw( const Matrix4f% mat );
		Matrix4fw( const Matrix4fw% mat );
		~Matrix4fw();

		const Matrix4f* GetRef();
		
		float		Get( int index );
		void		Set( int index, float value );

		static Matrix4fw^ Matrix4fw::operator + ( Matrix4fw^ mat0, Matrix4fw^ mat1 );
		Matrix4fw^	Add( const Matrix4fw^ mat );

		static Matrix4fw^ Matrix4fw::operator * ( Matrix4fw^ mat0, Matrix4fw^ mat1 );
		Matrix4fw^	Mul( const Matrix4fw^ mat );
		
		void		Zero();
		void		Identity();
		
		void		Translate( Vector3fw^ v );
		
		void		Rotate( Quatfw^ q );
		void		Rotate( Vector3fw^ v );
		void		RotateX( float degrees );
		void		RotateY( float degrees );
		void		RotateZ( float degrees );
		
		void		Scale( float _scale );
		void		Scale( Vector3fw^ _scale );
		
		void		LookAtView( Vector3fw^ pos, Vector3fw^ lookAt, Vector3fw^ up );
		void		BillboardAxis( Vector3fw^ posBB, Vector3fw^ posCamera, Vector3fw^ up );
		void		BillboardWorld( Vector3fw^ posBB, Vector3fw^ posCamera, Vector3fw^ up );
		
		float		Det();
		void		Transpose();
		void		InverseTrans();
		Matrix4fw^	InverseView();
		Matrix4fw^	Inverse();

		void		ProjectionOrthographic( float windowWidth, float windowHeight );
		void		ProjectionPerspective( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV );

		Vector3fw^	Transform( Vector3fw^ v );

		Vector3fw^	Forward();
		Vector3fw^	Up();
		Vector3fw^	Right();
	};
}}
