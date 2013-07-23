#include "Quatfw.h"

namespace Sentinel { namespace Math
{
	Quatfw::Quatfw()
	{
		mRef = new Quatf();
	}

	Quatfw::Quatfw( float _pitch, float _yaw, float _roll )
	{
		mRef = new Quatf( _pitch, _yaw, _roll );
	}

	Quatfw::Quatfw( float _x, float _y, float _z, float _w )
	{
		mRef = new Quatf( _x, _y, _z, _w );
	}

	Quatfw::Quatfw( const Quatf% q )
	{
		mRef = new Quatf( q );
	}

	Quatfw::Quatfw( const Quatfw% q )
	{
		mRef = new Quatf( *q.mRef );
	}

	Quatfw::~Quatfw()
	{
		delete mRef;
	}

	const Quatf* Quatfw::GetRef()
	{
		return mRef;
	}

	float Quatfw::X()
	{
		return mRef->X();
	}

	float Quatfw::Y()
	{
		return mRef->Y();
	}

	float Quatfw::Z()
	{
		return mRef->Z();
	}

	float Quatfw::W()
	{
		return mRef->W();
	}

	void Quatfw::SetX( float _x )
	{
		mRef->SetX( _x );
	}

	void Quatfw::SetY( float _y )
	{
		mRef->SetY( _y );
	}

	void Quatfw::SetZ( float _z )
	{
		mRef->SetZ( _z );
	}

	void Quatfw::SetW( float _w )
	{
		mRef->SetW( _w );
	}

	Quatfw^	Quatfw::operator * ( const Quatfw^ q )
	{
		return Mul( q );
	}

	Quatfw^	Quatfw::Mul( const Quatfw^ q )
	{
		return gcnew Quatfw( mRef->Mul( *q->mRef ));
	}

	Quatfw^ Quatfw::Mul( float scalar )
	{
		return gcnew Quatfw( mRef->Mul( scalar ));
	}

	float Quatfw::Length()
	{
		return mRef->Length();
	}

	float Quatfw::LengthSquared()
	{
		return mRef->LengthSquared();
	}

	float Quatfw::LengthManhattan()
	{
		return mRef->LengthManhattan();
	}

	Quatfw^ Quatfw::Normalize()
	{
		return gcnew Quatfw( mRef->Normalize() );
	}

	Quatfw^	Quatfw::AxisAngle( float _x, float _y, float _z, float _degrees )
	{
		mRef->AxisAngle( _x, _y, _z, _degrees );
		return this;
	}

	Quatfw^	Quatfw::AxisAngle()
	{
		mRef->AxisAngle();
		return this;
	}

	Quatfw^ Quatfw::Euler( float _pitch, float _yaw, float _roll )
	{
		mRef->Euler( _pitch, _yaw, _roll );
		return this;
	}

	Quatfw^ Quatfw::Euler()
	{
		mRef->Euler();
		return this;
	}
}}
