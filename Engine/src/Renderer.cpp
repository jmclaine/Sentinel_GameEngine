#include "Renderer.h"
#include "tinyxml.h"

namespace Sentinel
{
	float Renderer::SCREEN_WIDTH_RATIO	= 1;
	float Renderer::SCREEN_HEIGHT_RATIO	= 1;

	UINT Renderer::DESKTOP_WIDTH		= 1920;
	UINT Renderer::DESKTOP_HEIGHT		= 1080;

	UINT Renderer::SCREEN_WIDTH			= 1920;
	UINT Renderer::SCREEN_HEIGHT		= 1080;
	bool Renderer::FULLSCREEN			= false;

	Texture* Renderer::NULL_TEXTURE		= NULL;
	Texture* Renderer::BASE_TEXTURE		= NULL;

	int CreateRenderer( const char* filename )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
			return 0;

		TiXmlHandle	hDoc( &doc );

		TiXmlElement* pElem = hDoc.FirstChild( "Renderer" ).Element();
		const char *pName = pElem->Attribute( "Type" );

		if( strcmp( "DIRECTX", pName ) == 0 )
			Renderer::Inst( BuildRendererDX );
		else
			Renderer::Inst( BuildRendererGL );
			
		pElem->QueryBoolAttribute(		"Fullscreen",	&Renderer::FULLSCREEN );
		pElem->QueryUnsignedAttribute(	"Width",		&Renderer::SCREEN_WIDTH );
		pElem->QueryUnsignedAttribute(	"Height",		&Renderer::SCREEN_HEIGHT );

		Renderer::SCREEN_WIDTH_RATIO  = (float)Renderer::SCREEN_WIDTH  / (float)Renderer::SCREEN_WIDTH_BASE;
		Renderer::SCREEN_HEIGHT_RATIO = (float)Renderer::SCREEN_HEIGHT / (float)Renderer::SCREEN_HEIGHT_BASE;

		RECT screenRect;
		GetWindowRect( GetDesktopWindow(), &screenRect );
		Renderer::DESKTOP_WIDTH  = screenRect.right;
		Renderer::DESKTOP_HEIGHT = screenRect.bottom;

		return 1;
	}
}
