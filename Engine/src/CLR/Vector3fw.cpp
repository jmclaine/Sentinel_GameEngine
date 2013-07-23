#include "Vector3fw.h"

namespace Sentinel { namespace Math
{
	Vector3fw::Vector3fw()
	{
		mRef = new Vector3f();
	}

	Vector3fw::Vector3fw( float _x, float _y, float _z )
	{
		mRef = new Vector3f( _x, _y, _z );
	}
	
	Vector3fw::Vector3fw( const float* v )
	{
		mRef = new Vector3f( v[0], v[1], v[2] );
	}

	Vector3fw::Vector3fw( const Vector3f% v )
	{
		mRef = new Vector3f( v );
	}

	Vector3fw::Vector3fw( const Vector3fw% v )
	{
		mRef = new Vector3f( *v.mRef );
	}

	Vector3fw::~Vector3fw()
	{
		delete mRef;
	}

	const Vector3f* Vector3fw::GetRef()
	{
		return mRef;
	}

	float Vector3fw::X()
	{
		return mRef->X();
	}

	float Vector3fw::Y()
	{
		return mRef->Y();
	}

	float Vector3fw::Z()
	{
		return mRef->Z();
	}

	void Vector3fw::SetX( float _x )
	{
		mRef->SetX( _x );
	}

	void Vector3fw::SetY( float _y )
	{
		mRef->SetY( _y );
	}

	void Vector3fw::SetZ( float _z )
	{
		mRef->SetZ( _z );
	}

	bool Vector3fw::operator == ( const Vector3fw^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	bool Vector3fw::operator != ( const Vector3fw^ v )
	{
		return !mRef->Equals( *v->mRef );
	}

	bool Vector3fw::Equals( const Vector3fw^ v )
	{
		return mRef->Equals( *v->mRef );
	}

	Vector3fw^ Vector3fw::operator += ( const Vector3fw^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	Vector3fw^ Vector3fw::operator + ( const Vector3fw^ v )
	{
		return Add( v );
	}

	Vector3fw^ Vector3fw::Add( const Vector3fw^ v )
	{
		return gcnew Vector3fw( mRef->Add( *v->mRef ));
	}

	Vector3fw^ Vector3fw::operator -= ( const Vector3fw^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	Vector3fw^ Vector3fw::operator - ( const Vector3fw^ v )
	{
		return Sub( v );
	}

	Vector3fw^ Vector3fw::Sub( const Vector3fw^ v )
	{
		return gcnew Vector3fw( mRef->Sub( *v->mRef ));
	}

	Vector3fw^ Vector3fw::operator *= ( float scalar )
	{
		*mRef *= scalar;
		return this;
	}

	Vector3fw^ Vector3fw::operator * ( float scalar )
	{
		return Mul( scalar );
	}

	Vector3fw^ Vector3fw::Mul( float scalar )
	{
		return gcnew Vector3fw( mRef->Mul( scalar ));
	}

	Vector3fw^ Vector3fw::operator *= ( const Vector3fw^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	Vector3fw^ Vector3fw::operator * ( const Vector3fw^ v )
	{
		return Mul( v );
	}

	Vector3fw^ Vector3fw::Mul( const Vector3fw^ v )
	{
		return gcnew Vector3fw( mRef->Mul( *v->mRef ));
	}

	float Vector3fw::Length()
	{
		return mRef->Length();
	}

	float Vector3fw::LengthSquared()
	{
		return mRef->LengthSquared();
	}

	float Vector3fw::LengthManhattan()
	{
		return mRef->LengthManhattan();
	}

	Vector3fw^ Vector3fw::Normalize()
	{
		return gcnew Vector3fw( mRef->Normalize() );
	}

	float Vector3fw::Dot( const Vector3fw^ v )
	{
		return mRef->Dot( *v->mRef );
	}

	Vector3fw^ Vector3fw::Cross( const Vector3fw^ v )
	{
		return gcnew Vector3fw( mRef->Cross( *v->mRef ));
	}

	float Vector3fw::Angle( const Vector3fw^ v )
	{
		return mRef->Angle( *v->mRef );
	}
}}
