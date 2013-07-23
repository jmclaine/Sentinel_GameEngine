#pragma once

#include "Vector3f.h"

using namespace System;

namespace Sentinel { namespace Math
{
	public ref class Vector3fw
	{
	private:

		Vector3f*	mRef;

	public:

		Vector3fw();
		Vector3fw( float _x, float _y, float _z );
		Vector3fw( const float* v );
		Vector3fw( const Vector3f% v );
		Vector3fw( const Vector3fw% v );
		~Vector3fw();

		const Vector3f*	GetRef();

		float		X();
		float		Y();
		float		Z();

		void		SetX( float _x );
		void		SetY( float _y );
		void		SetZ( float _z );

		bool		operator == ( const Vector3fw^ v );
		bool		operator != ( const Vector3fw^ v );
		bool		Equals( const Vector3fw^ v );
		
		Vector3fw^	operator += ( const Vector3fw^ v );
		Vector3fw^	operator +  ( const Vector3fw^ v );
		Vector3fw^	Add( const Vector3fw^ v );

		Vector3fw^	operator -= ( const Vector3fw^ v );
		Vector3fw^	operator -  ( const Vector3fw^ v );
		Vector3fw^	Sub( const Vector3fw^ v );

		Vector3fw^	operator *= ( float scalar );
		Vector3fw^	operator *  ( float scalar );
		Vector3fw^	Mul( float scalar );

		Vector3fw^	operator *= ( const Vector3fw^ v );
		Vector3fw^	operator *  ( const Vector3fw^ v );
		Vector3fw^	Mul( const Vector3fw^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		Vector3fw^	Normalize();

		float		Dot( const Vector3fw^ v );
		Vector3fw^	Cross( const Vector3fw^ v );

		float		Angle( const Vector3fw^ v );
	};
}}
