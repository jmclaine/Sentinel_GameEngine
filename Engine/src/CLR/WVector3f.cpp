#include "WVector3f.h"

namespace Sentinel { namespace Math
{
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

	WVector3f::~WVector3f()
	{
		delete mRef;
	}

	WVector3f::!WVector3f()
	{
		delete mRef;
	}

	Vector3f* WVector3f::GetRef()
	{
		return mRef;
	}

	WFloat^ WVector3f::X()
	{
		return gcnew WFloat( &mRef->x );
	}

	WFloat^ WVector3f::Y()
	{
		return gcnew WFloat( &mRef->y );
	}

	WFloat^ WVector3f::Z()
	{
		return gcnew WFloat( &mRef->z );
	}

	bool WVector3f::operator == ( const WVector3f^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	bool WVector3f::operator != ( const WVector3f^ v )
	{
		return !mRef->Equals( *v->mRef );
	}

	bool WVector3f::Equals( const WVector3f^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	WVector3f^ WVector3f::operator += ( const WVector3f^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	WVector3f^ WVector3f::operator + ( const WVector3f^ v )
	{
		return Add( v );
	}

	WVector3f^ WVector3f::Add( const WVector3f^ v )
	{
		return gcnew WVector3f( mRef->Add( *v->mRef ));
	}

	WVector3f^ WVector3f::operator -= ( const WVector3f^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	WVector3f^ WVector3f::operator - ( const WVector3f^ v )
	{
		return Sub( v );
	}

	WVector3f^ WVector3f::Sub( const WVector3f^ v )
	{
		return gcnew WVector3f( mRef->Sub( *v->mRef ));
	}

	WVector3f^ WVector3f::operator *= ( float scalar )
	{
		*mRef *= scalar;
		return this;
	}

	WVector3f^ WVector3f::operator * ( float scalar )
	{
		return Mul( scalar );
	}

	WVector3f^ WVector3f::Mul( float scalar )
	{
		return gcnew WVector3f( mRef->Mul( scalar ));
	}

	WVector3f^ WVector3f::operator *= ( const WVector3f^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	WVector3f^ WVector3f::operator * ( const WVector3f^ v )
	{
		return Mul( v );
	}

	WVector3f^ WVector3f::Mul( const WVector3f^ v )
	{
		return gcnew WVector3f( mRef->Mul( *v->mRef ));
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

	float WVector3f::Dot( const WVector3f^ v )
	{
		return mRef->Dot( *v->mRef );
	}

	WVector3f^ WVector3f::Cross( const WVector3f^ v )
	{
		return gcnew WVector3f( mRef->Cross( *v->mRef ));
	}

	float WVector3f::Angle( const WVector3f^ v )
	{
		return mRef->Angle( *v->mRef );
	}
}}
