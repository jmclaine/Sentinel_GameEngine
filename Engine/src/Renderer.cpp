#include "Renderer.h"
#include "tinyxml.h"
#include <iostream>
#include <fstream>

namespace Sentinel
{
	// This function should create and assign the resulting Renderer*
	// to the SingletonAbstract instance; however, doing so results
	// in the pointer being set back to NULL in the CLR wrapper.
	//
	const void* Renderer::Load( const char* filename, WindowInfo& info )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
			return NULL;

		TiXmlHandle	hDoc( &doc );

		TiXmlElement* pElem = hDoc.FirstChild( "Renderer" ).Element();
		const char*   pName = pElem->Attribute( "Type" );

		if( !Renderer::Inst( (strcmp( "DIRECTX", pName ) == 0) ? BuildRendererDX() : BuildRendererGL() ))
			return NULL;
		
		pElem->QueryBoolAttribute(		"Fullscreen",	&info.mFullscreen );
		pElem->QueryUnsignedAttribute(	"Width",		&info.mWidth );
		pElem->QueryUnsignedAttribute(	"Height",		&info.mHeight );

		return (void*)Renderer::Inst();
	}

	Texture* Renderer::CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips )
	{
		return CreateTextureFromMemory( 0, width, height, format, createMips );
	}
}
