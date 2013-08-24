#pragma once

#include "Color.h"
#include "RFloat.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	public ref class WColorRGBA
	{
	protected:

		ColorRGBA*		mRef;

	public:

		WColorRGBA();
		WColorRGBA( float red, float green, float blue, float alpha );
		WColorRGBA( ColorRGBA& color );
		WColorRGBA( WColorRGBA^ color );
		WColorRGBA( WColorRGBA% color );
		~WColorRGBA();
		!WColorRGBA();

	protected:

		virtual void Delete();

	public:

		ColorRGBA*	GetRef();

		////////////////////////////////

		RFloat^		R();
		RFloat^		G();
		RFloat^		B();
		RFloat^		A();

		float*		Ptr();

		UINT		ToUINT();
	};

	public ref class RColorRGBA sealed : public WColorRGBA
	{
	public:

		RColorRGBA( ColorRGBA* color );
		RColorRGBA( WColorRGBA^ color );

		void		Set( const ColorRGBA& color );
		void		Set( WColorRGBA^ color );

	protected:

		virtual void Delete() override;
	};
}}
