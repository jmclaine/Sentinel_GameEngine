#include "WQuatf.h"

namespace Sentinel { namespace Math
{
	WQuatf::WQuatf()
	{
		mRef = new Quatf();
	}

	WQuatf::WQuatf( float _pitch, float _yaw, float _roll )
	{
		mRef = new Quatf( _pitch, _yaw, _roll );
	}

	WQuatf::WQuatf( float _x, float _y, float _z, float _w )
	{
		mRef = new Quatf( _x, _y, _z, _w );
	}

	WQuatf::WQuatf( const Quatf% q )
	{
		mRef = new Quatf( q );
	}

	WQuatf::WQuatf( const WQuatf% q )
	{
		mRef = new Quatf( *q.mRef );
	}

	WQuatf::~WQuatf()
	{
		delete mRef;
	}

	WQuatf::!WQuatf()
	{
		delete mRef;
	}

	Quatf* WQuatf::GetRef()
	{
		return mRef;
	}

	WFloat^ WQuatf::X()
	{
		return gcnew WFloat( &mRef->x );
	}

	WFloat^ WQuatf::Y()
	{
		return gcnew WFloat( &mRef->y );
	}

	WFloat^ WQuatf::Z()
	{
		return gcnew WFloat( &mRef->z );
	}

	WFloat^ WQuatf::W()
	{
		return gcnew WFloat( &mRef->w );
	}

	WQuatf^	WQuatf::operator * ( const WQuatf^ q )
	{
		return Mul( q );
	}

	WQuatf^	WQuatf::Mul( const WQuatf^ q )
	{
		return gcnew WQuatf( mRef->Mul( *q->mRef ));
	}

	WQuatf^ WQuatf::Mul( float scalar )
	{
		return gcnew WQuatf( mRef->Mul( scalar ));
	}

	float WQuatf::Length()
	{
		return mRef->Length();
	}

	float WQuatf::LengthSquared()
	{
		return mRef->LengthSquared();
	}

	float WQuatf::LengthManhattan()
	{
		return mRef->LengthManhattan();
	}

	WQuatf^ WQuatf::Normalize()
	{
		return gcnew WQuatf( mRef->Normalize() );
	}

	WQuatf^	WQuatf::AxisAngle( float _x, float _y, float _z, float _degrees )
	{
		mRef->AxisAngle( _x, _y, _z, _degrees );
		return this;
	}

	WQuatf^	WQuatf::AxisAngle()
	{
		mRef->AxisAngle();
		return this;
	}

	WQuatf^ WQuatf::Euler( float _pitch, float _yaw, float _roll )
	{
		mRef->Euler( _pitch, _yaw, _roll );
		return this;
	}

	WQuatf^ WQuatf::Euler()
	{
		mRef->Euler();
		return this;
	}
}}
