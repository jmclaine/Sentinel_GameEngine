#pragma once

#include "Vector3f.h"
#include "RFloat.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Math
{
	public ref class WVector3f
	{
	protected:

		Vector3f*	mRef;

	public:

		WVector3f();
		WVector3f( float _x, float _y, float _z );
		WVector3f( const float* v );
		WVector3f( const Vector3f% v );
		WVector3f( const WVector3f% v );
		~WVector3f();
		!WVector3f();

	protected:

		virtual void Delete();

	public:

		Vector3f*	GetRef();

		RFloat^		X();
		RFloat^		Y();
		RFloat^		Z();

		////////////////////////////////

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

	public ref class RVector3f sealed : public WVector3f
	{
	public:

		RVector3f( Vector3f* v );
		RVector3f( WVector3f^ v );

		void		Set( const Vector3f& v );
		void		Set( WVector3f^ v );
		
	protected:

		virtual void Delete() override;
	};
}}
