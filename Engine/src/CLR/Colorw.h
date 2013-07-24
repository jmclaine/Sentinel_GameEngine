#pragma once

#include "Color.h"

namespace Sentinel { namespace Assets
{
	public ref class ColorRGBAw
	{
	private:

		ColorRGBA*	mRef;

	public:

		ColorRGBAw();
		ColorRGBAw( ColorRGBAw% color );
		ColorRGBAw( ColorRGBA& color );
		~ColorRGBAw();

		ColorRGBA*	GetRef();

		float		R();
		float		G();
		float		B();
		float		A();

		void		SetR( float red );
		void		SetG( float green );
		void		SetB( float blue );
		void		SetA( float alpha );

		float*		Ptr();
	};
}}
