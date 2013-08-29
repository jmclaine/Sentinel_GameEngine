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
		Delete();
	}

	WQuatf::!WQuatf()
	{
		Delete();
	}

	void WQuatf::Delete()
	{
		delete mRef;
	}

	Quatf* WQuatf::GetRef()
	{
		return mRef;
	}

	////////////////////////////////

	DEFINE_PROPERTY( Quatf, float, x );
	DEFINE_PROPERTY( Quatf, float, y );
	DEFINE_PROPERTY( Quatf, float, z );
	DEFINE_PROPERTY( Quatf, float, w );

	////////////////////////////////

	WQuatf^ WQuatf::operator * ( WQuatf^ q0, WQuatf^ q1 )
	{
		return q0->Mul( q1 );
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

	WVector3f^ WQuatf::ToEuler()
	{
		return gcnew WVector3f( mRef->ToEuler() );
	}

	////////////////////////////////

	RQuatf::RQuatf( Quatf* q )
	{
		mRef = q;
	}
	
	RQuatf::RQuatf( WQuatf^ q )
	{
		mRef = q->GetRef();
	}

	void RQuatf::Set( const Quatf& q )
	{
		*mRef = q;
	}

	void RQuatf::Set( WQuatf^ q )
	{
		*mRef = *q->GetRef();
	}

	void RQuatf::Delete()
	{}
}}
