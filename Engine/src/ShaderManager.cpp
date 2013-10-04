#include "ShaderManager.h"
#include "Renderer.h"
#include "tinyxml.h"

namespace Sentinel
{
	ShaderManager::ShaderManager()
	{}

	ShaderManager::~ShaderManager()
	{}

	ShaderManager* ShaderManager::Create()
	{
		return ShaderManager::Inst();
	}

	void ShaderManager::Save( Archive& archive )
	{}

	void ShaderManager::Load( Archive& archive )
	{}

	bool ShaderManager::LoadConfig( const char* filename )
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

			std::shared_ptr< Shader > shader = ShaderManager::Inst()->Add( pName, Renderer::Inst()->CreateShader( pFile, pAttr, pUnif ));
			
			if( !shader )
				return false;

			pElem = pElem->NextSiblingElement();
		}

		return true;
	}
}
