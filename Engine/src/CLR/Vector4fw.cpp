#include "Vector4fw.h"

namespace Sentinel { namespace Math
{
	Vector4fw::Vector4fw()
	{
		mRef = new Vector4f();
	}

	Vector4fw::Vector4fw( float _x, float _y, float _z, float _w )
	{
		mRef = new Vector4f( _x, _y, _z, _w );
	}
	
	Vector4fw::Vector4fw( const float* v )
	{
		mRef = new Vector4f( v[0], v[1], v[2], v[3] );
	}

	Vector4fw::Vector4fw( const Vector4f% v )
	{
		mRef = new Vector4f( v );
	}

	Vector4fw::Vector4fw( const Vector4fw% v )
	{
		mRef = new Vector4f( *v.mRef );
	}

	Vector4fw::~Vector4fw()
	{
		delete mRef;
	}

	const Vector4f* Vector4fw::GetRef()
	{
		return mRef;
	}

	float Vector4fw::X()
	{
		return mRef->X();
	}

	float Vector4fw::Y()
	{
		return mRef->Y();
	}

	float Vector4fw::Z()
	{
		return mRef->Z();
	}

	float Vector4fw::W()
	{
		return mRef->W();
	}

	void Vector4fw::SetX( float _x )
	{
		mRef->SetX( _x );
	}

	void Vector4fw::SetY( float _y )
	{
		mRef->SetY( _y );
	}

	void Vector4fw::SetZ( float _z )
	{
		mRef->SetZ( _z );
	}

	void Vector4fw::SetW( float _w )
	{
		mRef->SetW( _w );
	}

	bool Vector4fw::operator == ( const Vector4fw^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	bool Vector4fw::operator != ( const Vector4fw^ v )
	{
		return !mRef->Equals( *v->mRef );
	}

	bool Vector4fw::Equals( const Vector4fw^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	Vector4fw^ Vector4fw::operator += ( const Vector4fw^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	Vector4fw^ Vector4fw::operator + ( const Vector4fw^ v )
	{
		return Add( v );
	}

	Vector4fw^ Vector4fw::Add( const Vector4fw^ v )
	{
		return gcnew Vector4fw( mRef->Add( *v->mRef ));
	}

	Vector4fw^ Vector4fw::operator -= ( const Vector4fw^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	Vector4fw^ Vector4fw::operator - ( const Vector4fw^ v )
	{
		return Sub( v );
	}

	Vector4fw^ Vector4fw::Sub( const Vector4fw^ v )
	{
		return gcnew Vector4fw( mRef->Sub( *v->mRef ));
	}

	Vector4fw^ Vector4fw::operator *= ( float scalar )
	{
		*mRef *= scalar;
		return this;
	}

	Vector4fw^ Vector4fw::operator * ( float scalar )
	{
		return Mul( scalar );
	}

	Vector4fw^ Vector4fw::Mul( float scalar )
	{
		return gcnew Vector4fw( mRef->Mul( scalar ));
	}

	Vector4fw^ Vector4fw::operator *= ( const Vector4fw^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	Vector4fw^ Vector4fw::operator * ( const Vector4fw^ v )
	{
		return Mul( v );
	}

	Vector4fw^ Vector4fw::Mul( const Vector4fw^ v )
	{
		return gcnew Vector4fw( mRef->Mul( *v->mRef ));
	}

	float Vector4fw::Length()
	{
		return mRef->Length();
	}

	float Vector4fw::LengthSquared()
	{
		return mRef->LengthSquared();
	}

	float Vector4fw::LengthManhattan()
	{
		return mRef->LengthManhattan();
	}

	Vector4fw^ Vector4fw::Normalize()
	{
		return gcnew Vector4fw( mRef->Normalize() );
	}

	float Vector4fw::Dot( const Vector4fw^ v )
	{
		return mRef->Dot( *v->mRef );
	}

	Vector4fw^ Vector4fw::Cross( const Vector4fw^ v )
	{
		return gcnew Vector4fw( mRef->Cross( *v->mRef ));
	}

	float Vector4fw::Angle( const Vector4fw^ v )
	{
		return mRef->Angle( *v->mRef );
	}
}}
