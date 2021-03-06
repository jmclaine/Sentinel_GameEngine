#pragma once
/*
All frames should be in texture coordinates, i.e. 0.0f - 1.0f

Example:

sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(0, 0, 64, 64), texture->Width(), texture->Height()));
*/
#include <memory>
#include <vector>

#include "Sentinel.h"
#include "Point.h"

namespace Sentinel
{
	class Texture;
	class Shader;

	class SENTINEL_DLL Sprite
	{
	protected:
		std::vector<Quad> mFrame;

	public:
		Sprite();
		~Sprite();

		//////////////////////////////////////

		void AddFrame(const Quad& coords);
		void RemoveFrame(UINT index);
		Quad& GetFrame(UINT index);
		UINT NumFrames();

		//////////////////////////////////////

		static Quad QUADtoTEXCOORD(const Quad& coords, UINT textureWidth, UINT textureHeight);
	};
}
