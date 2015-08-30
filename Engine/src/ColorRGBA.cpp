#include "ColorRGBA.h"

namespace Sentinel
{
	const ColorRGBA& ColorRGBA::WHITE = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	const ColorRGBA& ColorRGBA::BLACK = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	const ColorRGBA& ColorRGBA::RED = ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
	const ColorRGBA& ColorRGBA::GREEN = ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
	const ColorRGBA& ColorRGBA::BLUE = ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);
	const ColorRGBA& ColorRGBA::YELLOW = ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
	const ColorRGBA& ColorRGBA::MAGENTA = ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f);

	ColorRGBA::ColorRGBA() :
		r(1), g(1), b(1), a(1)
	{}

	ColorRGBA::ColorRGBA(float red, float green, float blue, float alpha) :
		r(red), g(green), b(blue), a(alpha)
	{}

	float* ColorRGBA::Ptr()
	{
		return static_cast<float*>(&r);
	}

	UINT ColorRGBA::ToUINT() const
	{
		return \
			((UINT)(a * 255.0f) << 24) |
			((UINT)(b * 255.0f) << 16) |
			((UINT)(g * 255.0f) << 8) |
			((UINT)(r * 255.0f));
	}

	bool ColorRGBA::operator == (const ColorRGBA& color) const
	{
		return Equals(color);
	}

	bool ColorRGBA::operator != (const ColorRGBA& color) const
	{
		return !Equals(color);
	}

	bool ColorRGBA::Equals(const ColorRGBA& color) const
	{
		return (r == color.r && g == color.g && b == color.b && a == color.a);
	}

	ColorRGBA& ColorRGBA::operator += (const ColorRGBA& color)
	{
		*this = Add(color);
		return *this;
	}

	ColorRGBA ColorRGBA::operator + (const ColorRGBA& color) const
	{
		return Add(color);
	}

	ColorRGBA ColorRGBA::Add(const ColorRGBA& color) const
	{
		return ColorRGBA(r + color.r, g + color.g, b + color.b, a + color.a);
	}

	ColorRGBA& ColorRGBA::operator -= (const ColorRGBA& color)
	{
		*this = Sub(color);
		return *this;
	}

	ColorRGBA ColorRGBA::operator - (const ColorRGBA& color) const
	{
		return Sub(color);
	}

	ColorRGBA ColorRGBA::Sub(const ColorRGBA& color) const
	{
		return ColorRGBA(r - color.r, g - color.g, b - color.b, a - color.a);
	}

	ColorRGBA& ColorRGBA::operator *= (float scalar)
	{
		*this = Mul(scalar);
		return *this;
	}

	ColorRGBA ColorRGBA::operator * (float scalar) const
	{
		return Mul(scalar);
	}

	ColorRGBA ColorRGBA::Mul(const float scalar) const
	{
		return ColorRGBA(r*scalar, g*scalar, b*scalar, a*scalar);
	}

	ColorRGBA& ColorRGBA::operator *= (const ColorRGBA& color)
	{
		*this = Mul(color);
		return *this;
	}

	ColorRGBA ColorRGBA::operator * (const ColorRGBA& color) const
	{
		return Mul(color);
	}

	ColorRGBA ColorRGBA::Mul(const ColorRGBA& color) const
	{
		return ColorRGBA(r*color.r, g*color.g, b*color.b, a*color.a);
	}
}
