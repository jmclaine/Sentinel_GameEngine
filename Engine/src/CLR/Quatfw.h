#pragma once

#include "Quatf.h"

using namespace System;

namespace Sentinel { namespace Math
{
	public ref class Quatfw
	{
	private:

		Quatf*		mRef;

	public:

		Quatfw();
		Quatfw( float _pitch, float _yaw, float _roll );
		Quatfw( float _x, float _y, float _z, float _w );
		Quatfw( const Quatf% q );
		Quatfw( const Quatfw% q );
		~Quatfw();

		const Quatf* GetRef();

		float		X();
		float		Y();
		float		Z();
		float		W();

		void		SetX( float _x );
		void		SetY( float _y );
		void		SetZ( float _z );
		void		SetW( float _w );

		Quatfw^		operator * ( const Quatfw^ q );
		Quatfw^		Mul( const Quatfw^ q );
		Quatfw^		Mul( float scalar );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		Quatfw^		Normalize();

		Quatfw^		AxisAngle( float _x, float _y, float _z, float _degrees );
		Quatfw^		AxisAngle();

		Quatfw^		Euler( float _pitch, float _yaw, float _roll );
		Quatfw^		Euler();
	};
}}
