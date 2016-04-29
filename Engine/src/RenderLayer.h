#pragma once

namespace Sentinel
{
	enum class RenderLayer : WORD
	{
		LAYER0 = 0x0001,
		LAYER1 = 0x0002,
		LAYER2 = 0x0004,
		LAYER3 = 0x0008,
		LAYER4 = 0x0010,
		LAYER5 = 0x0020,
		LAYER6 = 0x0040,
		LAYER7 = 0x0080,
		LAYER8 = 0x0100,
		LAYER9 = 0x0200,
		LAYER10 = 0x0400,
		LAYER11 = 0x0800,
		LAYER12 = 0x1000,
		LAYER13 = 0x2000,
		LAYER14 = 0x4000,
		LAYER15 = 0x8000,

		LENGTH = 16,
	};

	enum class RenderQueue : WORD
	{
		BACKGROUND = 0,
		GEOMETRY = 10000,
		TRANSPARENCY = 30000,
		FOREGROUND = 50000,
	};
}
