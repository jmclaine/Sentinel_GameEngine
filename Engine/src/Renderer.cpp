#include <iostream>
#include <fstream>
#include <Windows.h>

#include "Renderer.h"
#include "tinyxml.h"
#include "Shader.h"
#include "Texture.h"

namespace Sentinel
{
	WindowInfo::WindowInfo()
	{
		mFullscreen = false;

		mWidth = 1920;
		mHeight = 1080;

		mWidthRatio = 1.0f;
		mHeightRatio = 1.0f;
	}

	WindowInfo::~WindowInfo()
	{}

	bool WindowInfo::Fullscreen() const
	{
		return mFullscreen;
	}

	UINT WindowInfo::Width() const
	{
		return mWidth;
	}

	UINT WindowInfo::Height() const
	{
		return mHeight;
	}

	float WindowInfo::WidthRatio() const
	{
		return mWidthRatio;
	}

	float WindowInfo::HeightRatio() const
	{
		return mHeightRatio;
	}

	void* WindowInfo::Handle() const
	{
		return mHandle;
	}

	void WindowInfo::Update()
	{
		static RECT rect;
		GetClientRect((HWND)mHandle, &rect);

		mWidth = (UINT)(rect.right - rect.left);
		mHeight = (UINT)(rect.bottom - rect.top);

		mWidthRatio = (float)mWidth / (float)Renderer::WINDOW_WIDTH_BASE;
		mHeightRatio = (float)mHeight / (float)Renderer::WINDOW_HEIGHT_BASE;
	}

	////////////////////////////////////////////////////////////////////////////////////

	RenderTexture::RenderTexture(Texture* texture) :
		mTexture(texture)
	{}

	RenderTexture::~RenderTexture()
	{}

	Texture* RenderTexture::GetTexture() const
	{
		return mTexture;
	}

	////////////////////////////////////////////////////////////////////////////////////

	DepthStencil::DepthStencil(UINT width, UINT height) :
		mWidth(width),
		mHeight(height)
	{}

	DepthStencil::~DepthStencil()
	{}

	UINT DepthStencil::Width() const
	{
		return mWidth;
	}

	UINT DepthStencil::Height() const
	{
		return mHeight;
	}

	////////////////////////////////////////////////////////////////////////////////////

	BlendState::BlendState(
		BlendFormat::Type srcBlendColor, BlendFormat::Type dstBlendColor,
		BlendFormat::Type srcBlendAlpha, BlendFormat::Type dstBlendAlpha,
		BlendFunction::Type blendFuncColor, BlendFunction::Type blendFuncAlpha) :
		mSrcBlendColor(srcBlendColor), mDstBlendColor(dstBlendColor),
		mSrcBlendAlpha(srcBlendAlpha), mDstBlendAlpha(dstBlendAlpha),
		mBlendFuncColor(blendFuncColor), mBlendFuncAlpha(blendFuncAlpha)
	{}

	BlendFormat::Type BlendState::SrcBlendColor()
	{
		return mSrcBlendColor;
	}

	BlendFormat::Type BlendState::DstBlendColor()
	{
		return mDstBlendColor;
	}

	BlendFormat::Type BlendState::SrcBlendAlpha()
	{
		return mSrcBlendAlpha;
	}

	BlendFormat::Type BlendState::DstBlendAlpha()
	{
		return mDstBlendAlpha;
	}

	BlendFunction::Type BlendState::BlendFuncColor()
	{
		return mBlendFuncColor;
	}

	BlendFunction::Type BlendState::BlendFuncAlpha()
	{
		return mBlendFuncAlpha;
	}

	////////////////////////////////////////////////////////////////////////////////////

	UINT Renderer::WINDOW_WIDTH_BASE = 1920;
	UINT Renderer::WINDOW_HEIGHT_BASE = 1080;

	Renderer::Renderer() :
		mCurrShader(NULL)
	{}

	Renderer* Renderer::Create(const char* filename, WindowInfo& info)
	{
		TiXmlDocument doc;
		if (!doc.LoadFile(filename))
			return NULL;

		TiXmlHandle	hDoc(&doc);

		TiXmlElement* pElem = hDoc.FirstChild("Renderer").Element();
		const char* pName = pElem->Attribute("Type");

		Renderer* renderer = (strcmp("DIRECTX", pName) == 0) ? BuildRendererDX() : BuildRendererGL();

		if (!renderer)
			return NULL;

		pElem->QueryBoolAttribute("Fullscreen", &info.mFullscreen);
		pElem->QueryUnsignedAttribute("Width", &info.mWidth);
		pElem->QueryUnsignedAttribute("Height", &info.mHeight);

		Renderer::WINDOW_WIDTH_BASE = info.mWidth;
		Renderer::WINDOW_HEIGHT_BASE = info.mHeight;

		info.mWidthRatio = 1;
		info.mHeightRatio = 1;

		return renderer;
	}

	Texture* Renderer::CreateTextureFromResource(void* data, UINT length)
	{
		int width, height;
		int nChannels;

		unsigned char* pixels = stbi_load_from_memory((const unsigned char*)data, (int)length, &width, &height, &nChannels, 4);

		if (pixels == NULL)
			return NULL;

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
