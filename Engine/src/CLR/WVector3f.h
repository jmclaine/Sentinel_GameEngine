#pragma once

#include "Vector3f.h"

using namespace System;

namespace Sentinel { namespace Math
{
	public ref class WVector3f
	{
	private:

		Vector3f*	mRef;

	public:

		WVector3f();
		WVector3f( float _x, float _y, float _z );
		WVector3f( const float* v );
		WVector3f( const Vector3f% v );
		WVector3f( const WVector3f% v );
		~WVector3f();

		const Vector3f*	GetRef();

		float		X();
		float		Y();
		float		Z();

		void		SetX( float _x );
		void		SetY( float _y );
		void		SetZ( float _z );

		bool		operator == ( const WVector3f^ v );
		bool		operator != ( const WVector3f^ v );
		bool		Equals( const WVector3f^ v );
		
		WVector3f^	operator += ( const WVector3f^ v );
		WVector3f^	operator +  ( const WVector3f^ v );
		WVector3f^	Add( const WVector3f^ v );

		WVector3f^	operator -= ( const WVector3f^ v );
		WVector3f^	operator -  ( const WVector3f^ v );
		WVector3f^	Sub( const WVector3f^ v );

		WVector3f^	operator *= ( float scalar );
		WVector3f^	operator *  ( float scalar );
		WVector3f^	Mul( float scalar );

		WVector3f^	operator *= ( const WVector3f^ v );
		WVector3f^	operator *  ( const WVector3f^ v );
		WVector3f^	Mul( const WVector3f^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WVector3f^	Normalize();

		float		Dot( const WVector3f^ v );
		WVector3f^	Cross( const WVector3f^ v );

		float		Angle( const WVector3f^ v );
	};
}}
