#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	class Archive;
	class Renderer;

	enum class BlendFormat : BYTE
	{
		ZERO,
		ONE,

		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,

		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,

		COUNT,
		UNKNOWN
	};

	/////////////////////////////////

	enum class BlendFunction : BYTE
	{
		ADD,
		SUBTRACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX,

		COUNT,
		UNKNOWN
	};

	/////////////////////////////////

	class SENTINEL_DLL BlendState
	{
	protected:
		BlendFormat mSrcBlendColor;
		BlendFormat mDstBlendColor;
		BlendFunction mBlendFuncColor;

		BlendFormat mSrcBlendAlpha;
		BlendFormat mDstBlendAlpha;
		BlendFunction mBlendFuncAlpha;

		//////////////////////////////////

		BlendState(
			BlendFormat srcBlendColor, BlendFormat dstBlendColor, BlendFunction blendFuncColor,
			BlendFormat srcBlendAlpha, BlendFormat dstBlendAlpha, BlendFunction blendFuncAlpha);

	public:
		virtual ~BlendState() { }

		BlendFormat SrcBlendColor();
		BlendFormat DstBlendColor();
		BlendFunction BlendFuncColor();

		BlendFormat SrcBlendAlpha();
		BlendFormat DstBlendAlpha();
		BlendFunction BlendFuncAlpha();

		static void Save(
			Archive& archive,
			BlendState* blend);

		static BlendState* Load(
			Archive& archive,
			Renderer* renderer);
	};
}
