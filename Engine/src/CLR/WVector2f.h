#pragma once

#include "Vector2f.h"
#include "RFloat.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Math
{
	public ref class WVector2f
	{
	protected:

		Vector2f*	mRef;

	public:

		WVector2f();
		WVector2f( float _x, float _y );
		WVector2f( const float* v );
		WVector2f( const Vector2f% v );
		WVector2f( const WVector2f% v );
		~WVector2f();
		!WVector2f();

	protected:

		virtual void Delete();

	public:

		Vector2f*	GetRef();

		////////////////////////////////

		RFloat^		X();
		RFloat^		Y();

		////////////////////////////////
		
		bool		operator == ( const WVector2f^ v );
		bool		operator != ( const WVector2f^ v );
		bool		Equals( const WVector2f^ v );
		
		WVector2f^	operator += ( const WVector2f^ v );
		WVector2f^	operator +  ( const WVector2f^ v );
		WVector2f^	Add( const WVector2f^ v );

		WVector2f^	operator -= ( const WVector2f^ v );
		WVector2f^	operator -  ( const WVector2f^ v );
		WVector2f^	Sub( const WVector2f^ v );

		WVector2f^	operator *= ( float scalar );
		WVector2f^	operator *  ( float scalar );
		WVector2f^	Mul( float scalar );

		WVector2f^	operator *= ( const WVector2f^ v );
		WVector2f^	operator *  ( const WVector2f^ v );
		WVector2f^	Mul( const WVector2f^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WVector2f^	Normalize();

		float		Dot( const WVector2f^ v );
		WVector2f^	Cross();

		float		Angle( const WVector2f^ v );
	};

	public ref class RVector2f sealed : public WVector2f
	{
	public:

		RVector2f( Vector2f* v );
		RVector2f( WVector2f^ v );

		void		Set( const Vector2f& v );
		void		Set( WVector2f^ v );

	protected:

		virtual void Delete() override;
	};
}}
