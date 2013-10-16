#include "WVector4f.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF( Vector4f );

	WVector4f::WVector4f()
	{
		mRef = new Vector4f();
	}

	WVector4f::WVector4f( float _x, float _y, float _z, float _w )
	{
		mRef = new Vector4f( _x, _y, _z, _w );
	}
	
	WVector4f::WVector4f( const float* v )
	{
		mRef = new Vector4f( v[0], v[1], v[2], v[3] );
	}

	WVector4f::WVector4f( const Vector4f% v )
	{
		mRef = new Vector4f( v );
	}

	WVector4f::WVector4f( const WVector4f% v )
	{
		mRef = new Vector4f( *v.mRef );
	}

	////////////////////////////////

	DEFINE_PROPERTY( Vector4f, float, x );
	DEFINE_PROPERTY( Vector4f, float, y );
	DEFINE_PROPERTY( Vector4f, float, z );
	DEFINE_PROPERTY( Vector4f, float, w );

	////////////////////////////////

	DEFINE_OP_DEREF( Vector4f );

	bool WVector4f::operator == ( WVector4f^ v )
	{
		return mRef->Equals( v );
	}

	bool WVector4f::operator != ( WVector4f^ v )
	{
		return !mRef->Equals( v );
	}

	bool WVector4f::Equals( WVector4f^ v )
	{
		return mRef->Equals( v );
	}

	WVector4f^ WVector4f::operator += ( WVector4f^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	WVector4f^ WVector4f::operator + ( WVector4f^ v0, WVector4f^ v1 )
	{
		return v0->Add( v1 );
	}

	WVector4f^ WVector4f::Add( WVector4f^ v )
	{
		return gcnew WVector4f( mRef->Add( v ));
	}

	WVector4f^ WVector4f::operator -= ( WVector4f^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	WVector4f^ WVector4f::operator - ( WVector4f^ v0, WVector4f^ v1 )
	{
		return v0->Sub( v1 );
	}

	WVector4f^ WVector4f::Sub( WVector4f^ v )
	{
		return gcnew WVector4f( mRef->Sub( v ));
	}

	WVector4f^ WVector4f::operator *= ( float scalar )
	{
		*mRef *= scalar;
		return this;
	}

	WVector4f^ WVector4f::operator * ( WVector4f^ v, float scalar )
	{
		return v->Mul( scalar );
	}

	WVector4f^ WVector4f::operator * ( float scalar, WVector4f^ v )
	{
		return v->Mul( scalar );
	}

	WVector4f^ WVector4f::Mul( float scalar )
	{
		return gcnew WVector4f( mRef->Mul( scalar ));
	}

	WVector4f^ WVector4f::operator *= ( WVector4f^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	WVector4f^ WVector4f::operator * ( WVector4f^ v0, WVector4f^ v1 )
	{
		return v0->Mul( v1 );
	}

	WVector4f^ WVector4f::Mul( WVector4f^ v )
	{
		return gcnew WVector4f( mRef->Mul( v ));
	}

	float WVector4f::Length()
	{
		return mRef->Length();
	}

	float WVector4f::LengthSquared()
	{
		return mRef->LengthSquared();
	}

	float WVector4f::LengthManhattan()
	{
		return mRef->LengthManhattan();
	}

	WVector4f^ WVector4f::Normalize()
	{
		return gcnew WVector4f( mRef->Normalize() );
	}

	float WVector4f::Dot( WVector4f^ v )
	{
		return mRef->Dot( v );
	}

	WVector4f^ WVector4f::Cross( WVector4f^ v )
	{
		return gcnew WVector4f( mRef->Cross( v ));
	}

	float WVector4f::Angle( WVector4f^ v )
	{
		return mRef->Angle( v );
	}

	////////////////////////////////

	DEFINE_CLASS_REF( Vector4f );
}}
