#include "WVector2f.h"

namespace Sentinel { namespace Math
{
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

	WVector2f::~WVector2f()
	{
		Delete();
	}

	WVector2f::!WVector2f()
	{
		Delete();
	}

	void WVector2f::Delete()
	{
		delete mRef;
	}

	Vector2f* WVector2f::GetRef()
	{
		return mRef;
	}

	////////////////////////////////

	RFloat^ WVector2f::X()
	{
		return gcnew RFloat( &mRef->x );
	}

	RFloat^ WVector2f::Y()
	{
		return gcnew RFloat( &mRef->y );
	}

	////////////////////////////////

	bool WVector2f::operator == ( const WVector2f^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	bool WVector2f::operator != ( const WVector2f^ v )
	{
		return !mRef->Equals( *v->mRef );
	}

	bool WVector2f::Equals( const WVector2f^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	WVector2f^ WVector2f::operator += ( const WVector2f^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	WVector2f^ WVector2f::operator + ( WVector2f^ v0, WVector2f^ v1 )
	{
		return v0->Add( v1 );
	}

	WVector2f^ WVector2f::Add( const WVector2f^ v )
	{
		return gcnew WVector2f( mRef->Add( *v->mRef ));
	}

	WVector2f^ WVector2f::operator -= ( const WVector2f^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	WVector2f^ WVector2f::operator - ( WVector2f^ v0, WVector2f^ v1 )
	{
		return v0->Sub( v1 );
	}

	WVector2f^ WVector2f::Sub( const WVector2f^ v )
	{
		return gcnew WVector2f( mRef->Sub( *v->mRef ));
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

	WVector2f^ WVector2f::operator *= ( const WVector2f^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	WVector2f^ WVector2f::operator * ( WVector2f^ v0, WVector2f^ v1 )
	{
		return v0->Mul( v1 );
	}

	WVector2f^ WVector2f::Mul( const WVector2f^ v )
	{
		return gcnew WVector2f( mRef->Mul( *v->mRef ));
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

	float WVector2f::Dot( const WVector2f^ v )
	{
		return mRef->Dot( *v->mRef );
	}

	WVector2f^ WVector2f::Cross()
	{
		return gcnew WVector2f( mRef->Cross() );
	}

	float WVector2f::Angle( const WVector2f^ v )
	{
		return mRef->Angle( *v->mRef );
	}

	////////////////////////////////

	RVector2f::RVector2f( Vector2f* v )
	{
		mRef = v;
	}

	RVector2f::RVector2f( WVector2f^ v )
	{
		mRef = v->GetRef();
	}

	void RVector2f::Set( const Vector2f& v )
	{
		*mRef = v;
	}

	void RVector2f::Set( WVector2f^ v )
	{
		*mRef = *v->GetRef();
	}

	void RVector2f::Delete()
	{}
}}
