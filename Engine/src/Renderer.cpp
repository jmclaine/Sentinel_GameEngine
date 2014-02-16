#include "Renderer.h"
#include "tinyxml.h"
#include "Shader.h"

#include <iostream>
#include <fstream>

namespace Sentinel
{
	WindowInfo::WindowInfo()
	{
		mFullscreen		= false;

		mWidth			= 1920;
		mHeight			= 1080;

		mWidthRatio		= 1.0f;
		mHeightRatio	= 1.0f;
	}

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
		GetClientRect( (HWND)mHandle, &rect );

		mWidth  = (UINT)rect.right;
		mHeight = (UINT)rect.bottom;

		mWidthRatio  = (float)mWidth  / (float)Renderer::WINDOW_WIDTH_BASE;
		mHeightRatio = (float)mHeight / (float)Renderer::WINDOW_HEIGHT_BASE;
	}

	////////////////////////////////////////////////////////////////////////////////////

	BlendType BlendState::SrcBlendColor()
	{
		return mSrcBlendColor;
	}

	BlendType BlendState::DstBlendColor()
	{
		return mDstBlendColor;
	}

	BlendType BlendState::SrcBlendAlpha()
	{
		return mSrcBlendAlpha;
	}

	BlendType BlendState::DstBlendAlpha()
	{
		return mDstBlendAlpha;
	}

	BlendFuncType BlendState::BlendFuncColor()
	{
		return mBlendFuncColor;
	}

	BlendFuncType BlendState::BlendFuncAlpha()
	{
		return mBlendFuncAlpha;
	}

	////////////////////////////////////////////////////////////////////////////////////

	UINT Renderer::WINDOW_WIDTH_BASE  = 1920;
	UINT Renderer::WINDOW_HEIGHT_BASE = 1080;

	Renderer::Renderer() :
		mIsShaderLocked( false )
	{}

	Renderer* Renderer::Create( const char* filename, WindowInfo& info )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
			return NULL;

		TiXmlHandle	hDoc( &doc );

		TiXmlElement* pElem = hDoc.FirstChild( "Renderer" ).Element();
		const char*   pName = pElem->Attribute( "Type" );

		Renderer* renderer = (strcmp( "DIRECTX", pName ) == 0) ? BuildRendererDX() : BuildRendererGL();

		if( !renderer )
			return NULL;
		
		pElem->QueryBoolAttribute(		"Fullscreen",	&info.mFullscreen );
		pElem->QueryUnsignedAttribute(	"Width",		&info.mWidth );
		pElem->QueryUnsignedAttribute(	"Height",		&info.mHeight );

		Renderer::WINDOW_WIDTH_BASE  = info.mWidth;
		Renderer::WINDOW_HEIGHT_BASE = info.mHeight;

		info.mWidthRatio  = 1;
		info.mHeightRatio = 1;

		return renderer;
	}

	Texture* Renderer::CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return CreateTextureFromMemory( 0, width, height, format, createMips );
	}

	Texture* Renderer::CreateTextureFromResource( void* data, UINT length )
	{
		int width, height;
		int nChannels;

		unsigned char* pixels = stbi_load_from_memory( (const unsigned char*)data, (int)length, &width, &height, &nChannels, 4 );

		if( pixels == NULL )
			return NULL;

		return CreateTextureFromMemory( pixels, (UINT)width, (UINT)height, IMAGE_FORMAT_RGBA );
	}

	void Renderer::SetShader( Shader* shader )
	{
		if( !mIsShaderLocked )
		{
			if( mCurrShader )
				mCurrShader->Disable();

			mCurrShader = shader;

			mCurrShader->Enable();
		}
	}

	Shader* Renderer::GetShader()
	{
		return mCurrShader;
	}

	void Renderer::LockShader()
	{
		mIsShaderLocked = true;
	}

	void Renderer::UnlockShader()
	{
		mIsShaderLocked = false;
	}
}
