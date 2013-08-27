#pragma once

#include "Quatf.h"
#include "RFloat.h"
#include "WVector3f.h"

using namespace System;
using namespace Sentinel::Utilities;

namespace Sentinel { namespace Math
{
	public ref class WQuatf
	{
	protected:

		Quatf*		mRef;

	public:

		WQuatf();
		WQuatf( float _pitch, float _yaw, float _roll );
		WQuatf( float _x, float _y, float _z, float _w );
		WQuatf( const Quatf% q );
		WQuatf( const WQuatf% q );
		~WQuatf();
		!WQuatf();

	protected:

		virtual void Delete();

	public:

		Quatf*		GetRef();

		////////////////////////////////

		RFloat^		X();
		RFloat^		Y();
		RFloat^		Z();
		RFloat^		W();

		////////////////////////////////

		static WQuatf^ operator * ( WQuatf^ q0, WQuatf^ q1 );
		WQuatf^		Mul( const WQuatf^ q );
		WQuatf^		Mul( float scalar );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WQuatf^		Normalize();

		WQuatf^		AxisAngle( float _x, float _y, float _z, float _degrees );
		WQuatf^		AxisAngle();

		WQuatf^		Euler( float _pitch, float _yaw, float _roll );
		WQuatf^		Euler();
		WVector3f^	ToEuler();
	};

	public ref class RQuatf sealed : public WQuatf
	{
	public:

		RQuatf( Quatf* q );
		RQuatf( WQuatf^ q );

		void		Set( const Quatf& q );
		void		Set( WQuatf^ q );

	protected:

		virtual void Delete() override;
	};
}}
