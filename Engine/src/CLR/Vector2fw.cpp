#include "Vector2fw.h"

namespace Sentinel { namespace Math
{
	Vector2fw::Vector2fw()
	{
		mRef = new Vector2f();
	}

	Vector2fw::Vector2fw( float _x, float _y )
	{
		mRef = new Vector2f( _x, _y );
	}
	
	Vector2fw::Vector2fw( const float* v )
	{
		mRef = new Vector2f( v[0], v[1] );
	}

	Vector2fw::Vector2fw( const Vector2f% v )
	{
		mRef = new Vector2f( v );
	}

	Vector2fw::Vector2fw( const Vector2fw% v )
	{
		mRef = new Vector2f( *v.mRef );
	}

	Vector2fw::~Vector2fw()
	{
		delete mRef;
	}

	const Vector2f* Vector2fw::GetRef()
	{
		return mRef;
	}

	float Vector2fw::X()
	{
		return mRef->X();
	}

	float Vector2fw::Y()
	{
		return mRef->Y();
	}

	void Vector2fw::SetX( float _x )
	{
		mRef->SetX( _x );
	}

	void Vector2fw::SetY( float _y )
	{
		mRef->SetY( _y );
	}

	bool Vector2fw::operator == ( const Vector2fw^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	bool Vector2fw::operator != ( const Vector2fw^ v )
	{
		return !mRef->Equals( *v->mRef );
	}

	bool Vector2fw::Equals( const Vector2fw^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	Vector2fw^ Vector2fw::operator += ( const Vector2fw^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	Vector2fw^ Vector2fw::operator + ( const Vector2fw^ v )
	{
		return Add( v );
	}

	Vector2fw^ Vector2fw::Add( const Vector2fw^ v )
	{
		return gcnew Vector2fw( mRef->Add( *v->mRef ));
	}

	Vector2fw^ Vector2fw::operator -= ( const Vector2fw^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	Vector2fw^ Vector2fw::operator - ( const Vector2fw^ v )
	{
		return Sub( v );
	}

	Vector2fw^ Vector2fw::Sub( const Vector2fw^ v )
	{
		return gcnew Vector2fw( mRef->Sub( *v->mRef ));
	}

	Vector2fw^ Vector2fw::operator *= ( float scalar )
	{
		*mRef *= scalar;
		return this;
	}

	Vector2fw^ Vector2fw::operator * ( float scalar )
	{
		return Mul( scalar );
	}

	Vector2fw^ Vector2fw::Mul( float scalar )
	{
		return gcnew Vector2fw( mRef->Mul( scalar ));
	}

	Vector2fw^ Vector2fw::operator *= ( const Vector2fw^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	Vector2fw^ Vector2fw::operator * ( const Vector2fw^ v )
	{
		return Mul( v );
	}

	Vector2fw^ Vector2fw::Mul( const Vector2fw^ v )
	{
		return gcnew Vector2fw( mRef->Mul( *v->mRef ));
	}

	float Vector2fw::Length()
	{
		return mRef->Length();
	}

	float Vector2fw::LengthSquared()
	{
		return mRef->LengthSquared();
	}

	float Vector2fw::LengthManhattan()
	{
		return mRef->LengthManhattan();
	}

	Vector2fw^ Vector2fw::Normalize()
	{
		return gcnew Vector2fw( mRef->Normalize() );
	}

	float Vector2fw::Dot( const Vector2fw^ v )
	{
		return mRef->Dot( *v->mRef );
	}

	Vector2fw^ Vector2fw::Cross()
	{
		return gcnew Vector2fw( mRef->Cross() );
	}

	float Vector2fw::Angle( const Vector2fw^ v )
	{
		return mRef->Angle( *v->mRef );
	}
}}
