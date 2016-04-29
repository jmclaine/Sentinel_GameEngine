#include "DepthStencil.h"

namespace Sentinel
{
	DepthStencil::DepthStencil(UINT width, UINT height) :
		mWidth(width),
		mHeight(height)
	{ }

	DepthStencil::~DepthStencil()
	{ }

	UINT DepthStencil::Width() const
	{
		return mWidth;
	}

	UINT DepthStencil::Height() const
	{
		return mHeight;
	}
}
