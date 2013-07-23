#include "Renderer.h"
#include "tinyxml.h"

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

	int Renderer::Load( const char* filename )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
			return 0;

		TiXmlHandle	hDoc( &doc );

		TiXmlElement* pElem = hDoc.FirstChild( "Renderer" ).Element();
		const char*   pName = pElem->Attribute( "Type" );

		Renderer* result = BuildRendererGL();//(strcmp( "DIRECTX", pName ) == 0) ? BuildRendererDX() : BuildRendererGL();
		
		if( !result )
			return 0;

		pElem->QueryBoolAttribute(		"Fullscreen",	&Renderer::FULLSCREEN );
		pElem->QueryUnsignedAttribute(	"Width",		&Renderer::WINDOW_WIDTH );
		pElem->QueryUnsignedAttribute(	"Height",		&Renderer::WINDOW_HEIGHT );

		Renderer::Inst( result );

		return 1;
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
