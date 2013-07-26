#pragma once

#include "Color.h"

namespace Sentinel { namespace Assets
{
	public ref class WColorRGBA
	{
	private:

		ColorRGBA*	mRef;

	public:

		WColorRGBA();
		WColorRGBA( float red, float green, float blue, float alpha );
		WColorRGBA( ColorRGBA& color );
		WColorRGBA( WColorRGBA% color );
		~WColorRGBA();

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

		UINT		ToUINT();
	};
}}
