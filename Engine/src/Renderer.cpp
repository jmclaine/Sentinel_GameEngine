#include "Renderer.h"
#include "tinyxml.h"
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

	//////////////////////////////////////////

	// This function should create and assign the resulting Renderer*
	// to the SingletonAbstract instance; however, doing so results
	// in the pointer being set back to NULL in the CLR wrapper.
	//
	Renderer* Renderer::Create( const char* filename, WindowInfo& info )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
			return NULL;

		TiXmlHandle	hDoc( &doc );

		TiXmlElement* pElem = hDoc.FirstChild( "Renderer" ).Element();
		const char*   pName = pElem->Attribute( "Type" );

		if( !Renderer::Inst() )
			if( !Renderer::Inst( (strcmp( "DIRECTX", pName ) == 0) ? BuildRendererDX() : BuildRendererGL() ))
				return NULL;
		
		pElem->QueryBoolAttribute(		"Fullscreen",	&info.mFullscreen );
		pElem->QueryUnsignedAttribute(	"Width",		&info.mWidth );
		pElem->QueryUnsignedAttribute(	"Height",		&info.mHeight );

		return Renderer::Inst();
	}

	std::shared_ptr< Texture > Renderer::CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return CreateTextureFromMemory( 0, width, height, format, createMips );
	}
}
