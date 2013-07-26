#include "Color.h"

namespace Sentinel
{
	ColorRGBA::ColorRGBA( float red, float green, float blue, float alpha )
	{
		x = red;
		y = green;
		z = blue;
		w = alpha;
	}

	ColorRGBA::ColorRGBA( const Vector4f& v )
	{
		x = v.X();
		y = v.Y();
		z = v.Z();
		w = v.W();
	}

	float ColorRGBA::R() const
	{
		return x;
	}

	float ColorRGBA::G() const
	{
		return y;
	}

	float ColorRGBA::B() const
	{
		return z;
	}

	float ColorRGBA::A() const
	{
		return w;
	}

	void ColorRGBA::SetR( float red )
	{
		x = red;
	}

	void ColorRGBA::SetG( float green )
	{
		y = green;
	}

	void ColorRGBA::SetB( float blue )
	{
		z = blue;
	}

	void ColorRGBA::SetA( float alpha )
	{
		w = alpha;
	}

	float* ColorRGBA::Ptr()
	{
		return Vector4f::Ptr();
	}

	UINT ColorRGBA::ToUINT() const
	{
		return \
			((UINT)(A() * 255.0f) << 24) |
			((UINT)(B() * 255.0f) << 16) |
			((UINT)(G() * 255.0f) << 8)  |
			((UINT)(R() * 255.0f));
	}
	
	////////////////////////////////////////////////////////////////////

	ColorRGB::ColorRGB( float red, float green, float blue )
	{
		SetR( red );
		SetG( green );
		SetB( blue );
		SetA( 1.0f );
	}

	ColorRGB::ColorRGB( const Vector3f& v )
	{
		SetR( v.X() );
		SetG( v.Y() );
		SetB( v.Z() );
		SetA( 1.0f );
	}

	void ColorRGB::SetA( float alpha )
	{
		ColorRGBA::SetA( alpha );
	}

	////////////////////////////////////////////////////////////////////

	Material::Material( const ColorRGBA& ambient,
						const ColorRGBA& diffuse,
						const ColorRGBA& specular,
						float spec_comp ) :
		mAmbient( ambient ), mDiffuse( diffuse ), mSpecular( specular ), mSpecularComponent( spec_comp )
	{}
}
