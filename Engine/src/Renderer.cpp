#include "Renderer.h"
#include "tinyxml.h"
#include <iostream>
#include <fstream>

namespace Sentinel
{
	Renderer::Renderer()
	{
		FULLSCREEN		= false;
		WINDOW_WIDTH	= 1920;
		WINDOW_HEIGHT	= 1080;
	}

	// This function should create and assign the resulting Renderer*
	// to the SingletonAbstract instance; however, doing so results
	// in the pointer being set back to NULL in the CLR wrapper.
	//
	const void* Renderer::Load( const char* filename )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
			return NULL;

		TiXmlHandle	hDoc( &doc );

		TiXmlElement* pElem = hDoc.FirstChild( "Renderer" ).Element();
		const char*   pName = pElem->Attribute( "Type" );

		if( !Renderer::Inst( (strcmp( "DIRECTX", pName ) == 0) ? BuildRendererDX() : BuildRendererGL() ))
			return NULL;
		
		pElem->QueryBoolAttribute(		"Fullscreen",	&Renderer::Inst()->FULLSCREEN );
		pElem->QueryUnsignedAttribute(	"Width",		&Renderer::Inst()->WINDOW_WIDTH );
		pElem->QueryUnsignedAttribute(	"Height",		&Renderer::Inst()->WINDOW_HEIGHT );

		return (void*)Renderer::Inst();
	}

	UINT Renderer::Startup( void* hWnd )
	{
		WINDOW_WIDTH_RATIO  = (float)WINDOW_WIDTH  / (float)WINDOW_WIDTH_BASE;
		WINDOW_HEIGHT_RATIO = (float)WINDOW_HEIGHT / (float)WINDOW_HEIGHT_BASE;

		return 1;
	}

	Texture* Renderer::CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return CreateTextureFromMemory( 0, width, height, format, createMips );
	}
}
