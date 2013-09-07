#pragma once

#include "Matrix4f.h"
#include "WVector3f.h"
#include "WVector4f.h"
#include "WQuatf.h"

using namespace System;

namespace Sentinel { namespace Math
{
	public ref class WMatrix4f
	{
		DECLARE_REF( Matrix4f );

	public:

		WMatrix4f();
		WMatrix4f( const Matrix4f% mat );
		WMatrix4f( const WMatrix4f% mat );
		WMatrix4f( WQuatf^ q );

		////////////////////////////////
		
		property float default[ int ]
		{
			float get( int index );
			void  set( int index, float value );
		}

		////////////////////////////////

		DECLARE_OP_DEREF( Matrix4f );

		static WMatrix4f^ WMatrix4f::operator + ( WMatrix4f^ mat0, WMatrix4f^ mat1 );
		WMatrix4f^	Add( WMatrix4f^ mat );

		static WMatrix4f^ WMatrix4f::operator * ( WMatrix4f^ mat0, WMatrix4f^ mat1 );
		WMatrix4f^	Mul( WMatrix4f^ mat );
		
		void		Zero();
		void		Identity();
		
		void		Translate( WVector3f^ v );
		
		void		Rotate( WQuatf^ q );
		void		Rotate( WVector3f^ v );
		void		RotateX( float degrees );
		void		RotateY( float degrees );
		void		RotateZ( float degrees );
		
		void		Scale( float _scale );
		void		Scale( WVector3f^ _scale );
		
		void		LookAtView( WVector3f^ pos, WVector3f^ lookAt, WVector3f^ up );
		void		BillboardAxis( WVector3f^ posBB, WVector3f^ posCamera, WVector3f^ up );
		void		BillboardWorld( WVector3f^ posBB, WVector3f^ posCamera, WVector3f^ up );
		
		float		Det();
		void		Transpose();
		void		InverseTrans();
		WMatrix4f^	InverseView();
		WMatrix4f^	Inverse();

		void		ProjectionOrthographic( float windowWidth, float windowHeight );
		void		ProjectionPerspective( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV );

		WVector3f^	Transform( WVector3f^ v );
		WVector3f^	Transform( WVector4f^ v );

		WVector3f^	Forward();
		WVector3f^	Up();
		WVector3f^	Right();
	};

	DECLARE_CLASS_REF( Matrix4f );
}}
