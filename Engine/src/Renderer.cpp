#include <iostream>
#include <fstream>
#include <Windows.h>

#include "Renderer.h"
#include "tinyxml.h"
#include "Shader.h"
#include "Texture.h"
#include "WindowInfo.h"

namespace Sentinel
{
	Renderer::Renderer() :
		mCurrShader(nullptr)
	{ }

	Renderer* Renderer::Create(const char* filename, WindowInfo& info)
	{
		TiXmlDocument doc;
		if (!doc.LoadFile(filename))
			return nullptr;

		TiXmlHandle	hDoc(&doc);

		TiXmlElement* pElem = hDoc.FirstChild("Renderer").Element();
		const char* pName = pElem->Attribute("Type");

		Renderer* renderer = (strcmp("DIRECTX", pName) == 0) ? BuildRendererDX() : BuildRendererGL();

		if (!renderer)
			return nullptr;

		pElem->QueryBoolAttribute("Fullscreen", &info.mFullscreen);
		pElem->QueryUnsignedAttribute("Width", &info.mWidth);
		pElem->QueryUnsignedAttribute("Height", &info.mHeight);

		WindowInfo::BASE_WIDTH = info.mWidth;
		WindowInfo::BASE_HEIGHT = info.mHeight;

		info.mWidthRatio = 1;
		info.mHeightRatio = 1;

		return renderer;
	}

	Texture* Renderer::CreateTextureFromResource(void* data, UINT length)
	{
		int width, height;
		int nChannels;

		unsigned char* pixels = stbi_load_from_memory((const unsigned char*)data, (int)length, &width, &height, &nChannels, 4);

		if (pixels == nullptr)
			return nullptr;

		return CreateTexture(pixels, (UINT)width, (UINT)height, ImageFormat::RGBA);
	}

	void Renderer::SetShader(Shader* shader)
	{
		if (mCurrShader)
			mCurrShader->Disable();

		mCurrShader = shader;
		mCurrShader->Enable();
	}

	Shader* Renderer::GetShader()
	{
		return mCurrShader;
	}
}
