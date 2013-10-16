#include "WVector2f.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF( Vector2f );

	WVector2f::WVector2f()
	{
		mRef = new Vector2f();
	}

	WVector2f::WVector2f( float _x, float _y )
	{
		mRef = new Vector2f( _x, _y );
	}
	
	WVector2f::WVector2f( const float* v )
	{
		mRef = new Vector2f( v[0], v[1] );
	}

	WVector2f::WVector2f( const Vector2f% v )
	{
		mRef = new Vector2f( v );
	}

	WVector2f::WVector2f( const WVector2f% v )
	{
		mRef = new Vector2f( *v.mRef );
	}

	////////////////////////////////

	DEFINE_PROPERTY( Vector2f, float, x );
	DEFINE_PROPERTY( Vector2f, float, y );

	////////////////////////////////

	DEFINE_OP_DEREF( Vector2f );

	bool WVector2f::operator == ( WVector2f^ v )
	{
		return mRef->Equals( v );
	}

	bool WVector2f::operator != ( WVector2f^ v )
	{
		return !mRef->Equals( v );
	}

	bool WVector2f::Equals( WVector2f^ v )
	{
		return mRef->Equals( v );
	}

	WVector2f^ WVector2f::operator += ( WVector2f^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	WVector2f^ WVector2f::operator + ( WVector2f^ v0, WVector2f^ v1 )
	{
		return v0->Add( v1 );
	}

	WVector2f^ WVector2f::Add( WVector2f^ v )
	{
		return gcnew WVector2f( mRef->Add( v ));
	}

	WVector2f^ WVector2f::operator -= ( WVector2f^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	WVector2f^ WVector2f::operator - ( WVector2f^ v0, WVector2f^ v1 )
	{
		return v0->Sub( v1 );
	}

	WVector2f^ WVector2f::Sub( WVector2f^ v )
	{
		return gcnew WVector2f( mRef->Sub( v ));
	}

	WVector2f^ WVector2f::operator *= ( float scalar )
	{
		*mRef *= scalar;
		return this;
	}

	WVector2f^ WVector2f::operator * ( WVector2f^ v, float scalar )
	{
		return v->Mul( scalar );
	}

	WVector2f^ WVector2f::operator * ( float scalar, WVector2f^ v )
	{
		return v->Mul( scalar );
	}

	WVector2f^ WVector2f::Mul( float scalar )
	{
		return gcnew WVector2f( mRef->Mul( scalar ));
	}

	WVector2f^ WVector2f::operator *= ( WVector2f^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	WVector2f^ WVector2f::operator * ( WVector2f^ v0, WVector2f^ v1 )
	{
		return v0->Mul( v1 );
	}

	WVector2f^ WVector2f::Mul( WVector2f^ v )
	{
		return gcnew WVector2f( mRef->Mul( v ));
	}

	float WVector2f::Length()
	{
		return mRef->Length();
	}

	float WVector2f::LengthSquared()
	{
		return mRef->LengthSquared();
	}

	float WVector2f::LengthManhattan()
	{
		return mRef->LengthManhattan();
	}

	WVector2f^ WVector2f::Normalize()
	{
		return gcnew WVector2f( mRef->Normalize() );
	}

	float WVector2f::Dot( WVector2f^ v )
	{
		return mRef->Dot( v );
	}

	WVector2f^ WVector2f::Cross()
	{
		return gcnew WVector2f( mRef->Cross() );
	}

	float WVector2f::Angle( WVector2f^ v )
	{
		return mRef->Angle( v );
	}

	////////////////////////////////

	DEFINE_CLASS_REF( Vector2f );
}}
