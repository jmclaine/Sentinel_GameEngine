#include "WVector4f.h"

namespace Sentinel { namespace Math
{
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

	WVector4f::~WVector4f()
	{
		delete mRef;
	}

	WVector4f::!WVector4f()
	{
		delete mRef;
	}

	Vector4f* WVector4f::GetRef()
	{
		return mRef;
	}

	WFloat^ WVector4f::X()
	{
		return gcnew WFloat( &mRef->x );
	}

	WFloat^ WVector4f::Y()
	{
		return gcnew WFloat( &mRef->y );
	}

	WFloat^ WVector4f::Z()
	{
		return gcnew WFloat( &mRef->z );
	}

	WFloat^ WVector4f::W()
	{
		return gcnew WFloat( &mRef->w );
	}

	bool WVector4f::operator == ( const WVector4f^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	bool WVector4f::operator != ( const WVector4f^ v )
	{
		return !mRef->Equals( *v->mRef );
	}

	bool WVector4f::Equals( const WVector4f^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	WVector4f^ WVector4f::operator += ( const WVector4f^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	WVector4f^ WVector4f::operator + ( const WVector4f^ v )
	{
		return Add( v );
	}

	WVector4f^ WVector4f::Add( const WVector4f^ v )
	{
		return gcnew WVector4f( mRef->Add( *v->mRef ));
	}

	WVector4f^ WVector4f::operator -= ( const WVector4f^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	WVector4f^ WVector4f::operator - ( const WVector4f^ v )
	{
		return Sub( v );
	}

	WVector4f^ WVector4f::Sub( const WVector4f^ v )
	{
		return gcnew WVector4f( mRef->Sub( *v->mRef ));
	}

	WVector4f^ WVector4f::operator *= ( float scalar )
	{
		*mRef *= scalar;
		return this;
	}

	WVector4f^ WVector4f::operator * ( float scalar )
	{
		return Mul( scalar );
	}

	WVector4f^ WVector4f::Mul( float scalar )
	{
		return gcnew WVector4f( mRef->Mul( scalar ));
	}

	WVector4f^ WVector4f::operator *= ( const WVector4f^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	WVector4f^ WVector4f::operator * ( const WVector4f^ v )
	{
		return Mul( v );
	}

	WVector4f^ WVector4f::Mul( const WVector4f^ v )
	{
		return gcnew WVector4f( mRef->Mul( *v->mRef ));
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

	float WVector4f::Dot( const WVector4f^ v )
	{
		return mRef->Dot( *v->mRef );
	}

	WVector4f^ WVector4f::Cross( const WVector4f^ v )
	{
		return gcnew WVector4f( mRef->Cross( *v->mRef ));
	}

	float WVector4f::Angle( const WVector4f^ v )
	{
		return mRef->Angle( *v->mRef );
	}
}}
