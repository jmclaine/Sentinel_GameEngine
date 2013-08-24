#pragma once

#include "Vector4f.h"
#include "RFloat.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Math
{
	public ref class WVector4f
	{
	protected:

		Vector4f*	mRef;

	public:

		WVector4f();
		WVector4f( float _x, float _y, float _z, float _w );
		WVector4f( const float* v );
		WVector4f( const Vector4f% v );
		WVector4f( const WVector4f% v );
		~WVector4f();
		!WVector4f();

	protected:

		virtual void Delete();

	public:

		Vector4f*	GetRef();

		////////////////////////////////

		RFloat^		X();
		RFloat^		Y();
		RFloat^		Z();
		RFloat^		W();

		////////////////////////////////

		bool		operator == ( const WVector4f^ v );
		bool		operator != ( const WVector4f^ v );
		bool		Equals( const WVector4f^ v );
		
		WVector4f^	operator += ( const WVector4f^ v );
		WVector4f^	operator +  ( const WVector4f^ v );
		WVector4f^	Add( const WVector4f^ v );

		WVector4f^	operator -= ( const WVector4f^ v );
		WVector4f^	operator -  ( const WVector4f^ v );
		WVector4f^	Sub( const WVector4f^ v );

		WVector4f^	operator *= ( float scalar );
		WVector4f^	operator *  ( float scalar );
		WVector4f^	Mul( float scalar );

		WVector4f^	operator *= ( const WVector4f^ v );
		WVector4f^	operator *  ( const WVector4f^ v );
		WVector4f^	Mul( const WVector4f^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WVector4f^	Normalize();

		float		Dot( const WVector4f^ v );
		WVector4f^	Cross( const WVector4f^ v );

		float		Angle( const WVector4f^ v );
	};

	public ref class RVector4f sealed : public WVector4f
	{
	public:

		RVector4f( Vector4f* v );
		RVector4f( WVector4f^ v );

		void		Set( const Vector4f& v );
		void		Set( WVector4f^ v );

	protected:

		virtual void Delete() override;
	};
}}
