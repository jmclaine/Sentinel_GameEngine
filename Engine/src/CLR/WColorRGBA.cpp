#include "WColorRGBA.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF( ColorRGBA );

	WColorRGBA::WColorRGBA()
	{
		mRef = new ColorRGBA();
	}

	WColorRGBA::WColorRGBA( float red, float green, float blue, float alpha )
	{
		mRef = new ColorRGBA( red, green, blue, alpha );
	}

	WColorRGBA::WColorRGBA( ColorRGBA& color )
	{
		mRef = new ColorRGBA( color );
	}

	WColorRGBA::WColorRGBA( WColorRGBA^ color )
	{
		mRef = new ColorRGBA( *color->mRef );
	}

	WColorRGBA::WColorRGBA( WColorRGBA% color )
	{
		mRef = new ColorRGBA( *color.mRef );
	}

	////////////////////////////////

	DEFINE_PROPERTY( ColorRGBA, float, r );
	DEFINE_PROPERTY( ColorRGBA, float, g );
	DEFINE_PROPERTY( ColorRGBA, float, b );
	DEFINE_PROPERTY( ColorRGBA, float, a );

	////////////////////////////////

	DEFINE_OP_DEREF( ColorRGBA );

	float* WColorRGBA::Ptr()
	{
		return mRef->Ptr();
	}

	UINT WColorRGBA::ToUINT()
	{
		return mRef->ToUINT();
	}

	////////////////////////////////

	bool WColorRGBA::operator == ( WColorRGBA^ v )
	{
		return mRef->Equals( v );
	}

	bool WColorRGBA::operator != ( WColorRGBA^ v )
	{
		return !mRef->Equals( v );
	}

	bool WColorRGBA::Equals( WColorRGBA^ v )
	{
		return mRef->Equals( v );
	}

	WColorRGBA^ WColorRGBA::operator += ( WColorRGBA^ v )
	{
		*mRef += *v->mRef;
		return this;
	}

	WColorRGBA^ WColorRGBA::operator + ( WColorRGBA^ v0, WColorRGBA^ v1 )
	{
		return v0->Add( v1 );
	}

	WColorRGBA^ WColorRGBA::Add( WColorRGBA^ v )
	{
		return gcnew WColorRGBA( mRef->Add( v ));
	}

	WColorRGBA^ WColorRGBA::operator -= ( WColorRGBA^ v )
	{
		*mRef -= *v->mRef;
		return this;
	}

	WColorRGBA^ WColorRGBA::operator - ( WColorRGBA^ v0, WColorRGBA^ v1 )
	{
		return v0->Sub( v1 );
	}

	WColorRGBA^ WColorRGBA::Sub( WColorRGBA^ v )
	{
		return gcnew WColorRGBA( mRef->Sub( v ));
	}

	WColorRGBA^ WColorRGBA::operator *= ( float scalar )
	{
		*mRef *= scalar;
		return this;
	}

	WColorRGBA^ WColorRGBA::operator * ( WColorRGBA^ v, float scalar )
	{
		return v->Mul( scalar );
	}

	WColorRGBA^ WColorRGBA::operator * ( float scalar, WColorRGBA^ v )
	{
		return v->Mul( scalar );
	}

	WColorRGBA^ WColorRGBA::Mul( float scalar )
	{
		return gcnew WColorRGBA( mRef->Mul( scalar ));
	}

	WColorRGBA^ WColorRGBA::operator *= ( WColorRGBA^ v )
	{
		*mRef *= *v->mRef;
		return this;
	}

	WColorRGBA^ WColorRGBA::operator * ( WColorRGBA^ v0, WColorRGBA^ v1 )
	{
		return v0->Mul( v1 );
	}

	WColorRGBA^ WColorRGBA::Mul( WColorRGBA^ v )
	{
		return gcnew WColorRGBA( mRef->Mul( v ));
	}

	////////////////////////////////

	DEFINE_CLASS_REF( ColorRGBA );
}}