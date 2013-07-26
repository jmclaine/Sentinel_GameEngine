#pragma once

#include "Common.h"
#include "Vector3f.h"
#include "Vector4f.h"

namespace Sentinel
{
	class SENTINEL_DLL ColorRGBA : private Vector4f
	{
	public:

		ColorRGBA( float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f );
		ColorRGBA( const Vector4f& v );

		float	R() const;
		float	G() const;
		float	B() const;
		float	A() const;

		void	SetR( float red );
		void	SetG( float green );
		void	SetB( float blue );
		void	SetA( float alpha );

		float*	Ptr();

		UINT	ToUINT() const;
	};
	
	class SENTINEL_DLL ColorRGB : public ColorRGBA
	{
	public:

		ColorRGB( float red = 1.0f, float green = 1.0f, float blue = 1.0f );
		ColorRGB( const Vector3f& v );

	protected:

		void	SetA( float alpha );
	};

	class SENTINEL_DLL Material
	{
	public:

		ColorRGBA	mAmbient;
		ColorRGBA	mDiffuse;
		ColorRGBA	mSpecular;
		float		mSpecularComponent;

		Material( const ColorRGBA& ambient  = ColorRGBA( 0.2f, 0.2f, 0.2f, 1.0f ),
				  const ColorRGBA& diffuse  = ColorRGBA( 0.6f, 0.6f, 0.6f, 1.0f ),
				  const ColorRGBA& specular = ColorRGBA( 0.2f, 0.2f, 0.2f, 1.0f ),
				  float spec_comp = 32.0f );
	};
}
