#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL ColorRGBA
	{
	public:

		float r, g, b, a;

		/////////////////////////////

		ColorRGBA();
		ColorRGBA( float red, float green, float blue, float alpha = 1.0f );

		float*		Ptr();

		UINT		ToUINT() const;

		bool		operator == ( const ColorRGBA& color ) const;
		bool		operator != ( const ColorRGBA& color ) const;
		bool		Equals( const ColorRGBA& color ) const;

		ColorRGBA&	operator += ( const ColorRGBA& color );
		ColorRGBA	operator +  ( const ColorRGBA& color ) const;
		ColorRGBA	Add( const ColorRGBA& color ) const;

		ColorRGBA&	operator -= ( const ColorRGBA& color );
		ColorRGBA	operator -  ( const ColorRGBA& color ) const;
		ColorRGBA	Sub( const ColorRGBA& color ) const;

		ColorRGBA&	operator *= ( float scalar );
		ColorRGBA	operator *  ( float scalar ) const;
		ColorRGBA	Mul( float scalar ) const;

		ColorRGBA&	operator *= ( const ColorRGBA& color );
		ColorRGBA	operator *  ( const ColorRGBA& color ) const;
		ColorRGBA	Mul( const ColorRGBA& color ) const;

	public:

		static const ColorRGBA& WHITE;
		static const ColorRGBA& BLACK;
		static const ColorRGBA& RED;
		static const ColorRGBA& GREEN;
		static const ColorRGBA& BLUE;
		static const ColorRGBA& YELLOW;
		static const ColorRGBA& MAGENTA;
	};
}
