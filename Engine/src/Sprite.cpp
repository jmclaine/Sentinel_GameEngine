#include "Sprite.h"
#include "Texture.h"
#include "Vector2.h"
#include "Point.h"

namespace Sentinel
{
	Sprite::Sprite()
	{ }

	Sprite::~Sprite()
	{ }

	//////////////////////////////////////

	void Sprite::AddFrame(const Quad& coords)
	{
		mFrame.push_back(coords);
	}

	void Sprite::RemoveFrame(UINT index)
	{
		mFrame.erase(mFrame.begin() + index);
	}

	Quad& Sprite::GetFrame(UINT index)
	{
		return mFrame[index];
	}

	UINT Sprite::NumFrames()
	{
		return mFrame.size();
	}

	//////////////////////////////////////

	Quad Sprite::QUADtoTEXCOORD(const Quad& coords, UINT textureWidth, UINT textureHeight)
	{
		return Quad(
			coords.left / static_cast<float>(textureWidth),
			coords.top / static_cast<float>(textureHeight),
			coords.right / static_cast<float>(textureWidth),
			coords.bottom / static_cast<float>(textureHeight));
	}
}
