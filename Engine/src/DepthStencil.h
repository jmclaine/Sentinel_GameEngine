#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	enum class DepthFormat : BYTE
	{
		OFF,
		LESS,
		EQUAL,
		LEQUAL,
		GREATER,
		NOTEQUAL,
		GEQUAL,
		ALWAYS,

		COUNT,
		UNKNOWN
	};

	class DepthStencil
	{
	protected:
		UINT mWidth;
		UINT mHeight;

		//////////////////

		DepthStencil(UINT width, UINT height);

	public:
		virtual ~DepthStencil();

		UINT Width() const;
		UINT Height() const;
	};
}