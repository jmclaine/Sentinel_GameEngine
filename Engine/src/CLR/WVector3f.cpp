#include "WVector3f.h"

namespace Sentinel { namespace Math
{
	DEFINE_REF( Vector3f );

	WVector3f::WVector3f()
	{
		mRef = new Vector3f();
	}

	WVector3f::WVector3f( float _x, float _y, float _z )
	{
		mRef = new Vector3f( _x, _y, _z );
	}
	
	WVector3f::WVector3f( const float* v )
	{
		mRef = new Vector3f( v[0], v[1], v[2] );
	}

	WVector3f::WVector3f( const Vector3f% v )
	{
		mRef = new Vector3f( v );
	}

	WVector3f::WVector3f( const WVector3f% v )
	{
		mRef = new Vector3f( *v.mRef );
	}

	////////////////////////////////

	DEFINE_PROPERTY( Vector3f, float, x );
	DEFINE_PROPERTY( Vector3f, float, y );
	DEFINE_PROPERTY( Vector3f, float, z );

	////////////////////////////////

	DEFINE_OP_DEREF( Vector3f );

	bool WVector3f::operator == ( WVector3f^ v )
	{
		return mRef->Equals( v );
	}

	bool WVector3f::operator != ( WVector3f^ v )
	{
		return !mRef->Equals( v );
	}

	bool WVector3f::Equals( WVector3f^ v )
	{
		return mRef->Equals( v );
	}

	WVector3f^ WVector3f::operator += ( WVector3f^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	WVector3f^ WVector3f::operator + ( WVector3f^ v0, WVector3f^ v1 )
	{
		return v0->Add( v1 );
	}

	WVector3f^ WVector3f::Add( WVector3f^ v )
	{
		return gcnew WVector3f( mRef->Add( v ));
	}

	WVector3f^ WVector3f::operator -= ( WVector3f^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	WVector3f^ WVector3f::operator - ( WVector3f^ v0, WVector3f^ v1 )
	{
		return v0->Sub( v1 );
	}

	WVector3f^ WVector3f::Sub( WVector3f^ v )
	{
		return gcnew WVector3f( mRef->Sub( v ));
	}

	WVector3f^ WVector3f::operator *= ( float scalar )
	{
		*mRef *= scalar;
		return this;
	}

	WVector3f^ WVector3f::operator * ( WVector3f^ v, float scalar )
	{
		return v->Mul( scalar );
	}

	WVector3f^ WVector3f::operator * ( float scalar, WVector3f^ v )
	{
		return v->Mul( scalar );
	}

	WVector3f^ WVector3f::Mul( float scalar )
	{
		return gcnew WVector3f( mRef->Mul( scalar ));
	}

	WVector3f^ WVector3f::operator *= ( WVector3f^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	WVector3f^ WVector3f::operator * ( WVector3f^ v0, WVector3f^ v1 )
	{
		return v0->Mul( v1 );
	}

	WVector3f^ WVector3f::Mul( WVector3f^ v )
	{
		return gcnew WVector3f( mRef->Mul( v ));
	}

	float WVector3f::Length()
	{
		return mRef->Length();
	}

	float WVector3f::LengthSquared()
	{
		return mRef->LengthSquared();
	}

	float WVector3f::LengthManhattan()
	{
		return mRef->LengthManhattan();
	}

	WVector3f^ WVector3f::Normalize()
	{
		return gcnew WVector3f( mRef->Normalize() );
	}

	float WVector3f::Dot( WVector3f^ v )
	{
		return mRef->Dot( v );
	}

	WVector3f^ WVector3f::Cross( WVector3f^ v )
	{
		return gcnew WVector3f( mRef->Cross( v ));
	}

	float WVector3f::Angle( WVector3f^ v )
	{
		return mRef->Angle( v );
	}

	////////////////////////////////

	DEFINE_CLASS_REF( Vector3f );
}}
