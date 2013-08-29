#pragma once

#include "Color.h"
#include "Property.h"

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

		DECLARE_PROPERTY( float, r );
		DECLARE_PROPERTY( float, g );
		DECLARE_PROPERTY( float, b );
		DECLARE_PROPERTY( float, a );

		////////////////////////////////

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
