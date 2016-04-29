#include "BlendState.h"
#include "Renderer.h"
#include "Archive.h"

namespace Sentinel
{
	BlendState::BlendState(
		BlendFormat srcBlendColor, BlendFormat dstBlendColor, BlendFunction blendFuncColor,
		BlendFormat srcBlendAlpha, BlendFormat dstBlendAlpha, BlendFunction blendFuncAlpha) :
		mSrcBlendColor(srcBlendColor), mDstBlendColor(dstBlendColor), mBlendFuncColor(blendFuncColor),
		mSrcBlendAlpha(srcBlendAlpha), mDstBlendAlpha(dstBlendAlpha), mBlendFuncAlpha(blendFuncAlpha)
	{ }

	BlendFormat BlendState::SrcBlendColor()
	{
		return mSrcBlendColor;
	}

	BlendFormat BlendState::DstBlendColor()
	{
		return mDstBlendColor;
	}

	BlendFunction BlendState::BlendFuncColor()
	{
		return mBlendFuncColor;
	}

	BlendFormat BlendState::SrcBlendAlpha()
	{
		return mSrcBlendAlpha;
	}

	BlendFormat BlendState::DstBlendAlpha()
	{
		return mDstBlendAlpha;
	}

	BlendFunction BlendState::BlendFuncAlpha()
	{
		return mBlendFuncAlpha;
	}

	void BlendState::Save(
		Archive& archive,
		BlendState* blend)
	{
		BYTE type;

		type = (BYTE)blend->SrcBlendColor();
		archive.Write(&type);

		type = (BYTE)blend->DstBlendColor();
		archive.Write(&type);

		type = (BYTE)blend->BlendFuncColor();
		archive.Write(&type);

		type = (BYTE)blend->SrcBlendAlpha();
		archive.Write(&type);

		type = (BYTE)blend->DstBlendAlpha();
		archive.Write(&type);

		type = (BYTE)blend->BlendFuncAlpha();
		archive.Write(&type);
	}

	BlendState* BlendState::Load(
		Archive& archive,
		Renderer* renderer)
	{
		BYTE srcBlendColor;
		archive.Read(&srcBlendColor);

		BYTE dstBlendColor;
		archive.Read(&dstBlendColor);

		BYTE blendFuncColor;
		archive.Read(&blendFuncColor);

		BYTE srcBlendAlpha;
		archive.Read(&srcBlendAlpha);

		BYTE dstBlendAlpha;
		archive.Read(&dstBlendAlpha);

		BYTE blendFuncAlpha;
		archive.Read(&blendFuncAlpha);

		return renderer->CreateBlendState(
			(BlendFormat)srcBlendColor, (BlendFormat)dstBlendColor, (BlendFunction)blendFuncColor,
			(BlendFormat)srcBlendAlpha, (BlendFormat)dstBlendAlpha, (BlendFunction)blendFuncAlpha);
	}
}