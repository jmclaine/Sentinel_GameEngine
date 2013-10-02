#include "AssetManager.h"
#include "Renderer.h"
#include "tinyxml.h"

namespace Sentinel
{
#define DEFINE_MANAGER( refClass )\
	refClass##Manager::refClass##Manager() {}\
	refClass##Manager::~refClass##Manager() {}\
	refClass##Manager* refClass##Manager::Create() { return refClass##Manager::Inst(); }

#define DEFINE_MANAGER_EX( refClass )\
	DEFINE_MANAGER( refClass );\
	bool refClass##Manager::Load( const char* filename ) { return false; }

	DEFINE_MANAGER_EX( Texture );
	DEFINE_MANAGER( Shader );

	bool ShaderManager::Load( const char* filename )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
			return false;

		TiXmlHandle	hDoc( &doc );

		// Starting enclosure: <Shaders>
		//
		TiXmlElement* pMain = hDoc.FirstChild( "Shaders" ).Element();

		// Read <Definition>
		//
		TiXmlElement* pElem = pMain->FirstChild( "Definition" )->ToElement();
		
		while( pElem != NULL )
		{
			const char* pFile = pElem->Attribute( "FileName" );
			const char* pAttr = pElem->Attribute( "Attribute" );
			const char* pUnif = pElem->Attribute( "Uniform" );
			const char* pName = pElem->Attribute( "Name" );

			if( !ShaderManager::Inst()->Add( pName, Renderer::Inst()->CreateShader( pFile, pAttr, pUnif )))
				return false;

			pElem = pElem->NextSiblingElement();
		}

		return true;
	}
}
