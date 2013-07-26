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
		delete mRef;
	}

	const Vector2f* WVector2f::GetRef()
	{
		return mRef;
	}

	float WVector2f::X()
	{
		return mRef->X();
	}

	float WVector2f::Y()
	{
		return mRef->Y();
	}

	void WVector2f::SetX( float _x )
	{
		mRef->SetX( _x );
	}

	void WVector2f::SetY( float _y )
	{
		mRef->SetY( _y );
	}

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

	WVector2f^ WVector2f::operator + ( const WVector2f^ v )
	{
		return Add( v );
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

	WVector2f^ WVector2f::operator - ( const WVector2f^ v )
	{
		return Sub( v );
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

	WVector2f^ WVector2f::operator * ( float scalar )
	{
		return Mul( scalar );
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

	WVector2f^ WVector2f::operator * ( const WVector2f^ v )
	{
		return Mul( v );
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
}}
