#include "Renderer.h"
#include "tinyxml.h"
#include <iostream>
#include <fstream>

namespace Sentinel
{
	float		Renderer::WINDOW_WIDTH_RATIO	= 1;
	float		Renderer::WINDOW_HEIGHT_RATIO	= 1;

	UINT		Renderer::DESKTOP_WIDTH			= 1920;
	UINT		Renderer::DESKTOP_HEIGHT		= 1080;

	UINT		Renderer::WINDOW_WIDTH			= 1920;
	UINT		Renderer::WINDOW_HEIGHT			= 1080;
	bool		Renderer::FULLSCREEN			= false;

	Texture*	Renderer::NULL_TEXTURE			= NULL;
	Texture*	Renderer::BASE_TEXTURE			= NULL;

	// This function should create and assign the resulting Renderer*
	// to the SingletonAbstract instance; however, doing so results
	// in the pointer being set back to NULL in the CLR wrapper.
	//
	const Renderer* Renderer::Load( const char* filename )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
			return NULL;

		TiXmlHandle	hDoc( &doc );

		TiXmlElement* pElem = hDoc.FirstChild( "Renderer" ).Element();
		const char*   pName = pElem->Attribute( "Type" );

		if( !mSingle )
			mSingle = (strcmp( "DIRECTX", pName ) == 0) ? BuildRendererDX() : BuildRendererGL();
		
		pElem->QueryBoolAttribute(		"Fullscreen",	&Renderer::FULLSCREEN );
		pElem->QueryUnsignedAttribute(	"Width",		&Renderer::WINDOW_WIDTH );
		pElem->QueryUnsignedAttribute(	"Height",		&Renderer::WINDOW_HEIGHT );

		return const_cast< Renderer* >(mSingle);
	}

	UINT Renderer::Startup( void* hWnd, bool fullscreen, UINT width, UINT height )
	{
		WINDOW_WIDTH	= width;
		WINDOW_HEIGHT	= height;
		FULLSCREEN		= fullscreen;

		WINDOW_WIDTH_RATIO  = (float)WINDOW_WIDTH  / (float)WINDOW_WIDTH_BASE;
		WINDOW_HEIGHT_RATIO = (float)WINDOW_HEIGHT / (float)WINDOW_HEIGHT_BASE;

		RECT screenRect;
		GetWindowRect( GetDesktopWindow(), &screenRect );
		DESKTOP_WIDTH  = screenRect.right;
		DESKTOP_HEIGHT = screenRect.bottom;

		return 1;
	}

	Texture* Renderer::CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return CreateTextureFromMemory( 0, width, height, format, createMips );
	}
}
