#include "ShaderManager.h"
#include "Renderer.h"
#include "tinyxml.h"

namespace Sentinel
{
	ShaderManager::ShaderManager()
	{}

	ShaderManager::~ShaderManager()
	{
		TRAVERSE_LIST( it, mData )
			delete it->second;

		mData.clear();
	}

	ShaderManager* ShaderManager::Load( const char* filename )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
			return NULL;

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

			Result result = ShaderManager::Inst()->Add( Renderer::Inst()->CreateShader( pFile, pAttr, pUnif ), pName );
		
			if( result != SUCCESS )
				return NULL;

			pElem = pElem->NextSiblingElement();
		}

		return ShaderManager::Inst();
	}

	ShaderManager::Result ShaderManager::Add( Shader* shader, const std::string& name )
	{
		if( !shader )
			return ShaderManager::LOAD_FILE_FAILED;

		if( mData.find( name ) != mData.end() )
			return ShaderManager::DUPLICATE_NAME;

		mData.insert( std::pair< std::string, Shader* >( name, shader ));

		return ShaderManager::SUCCESS;
	}

	void ShaderManager::Remove( const std::string& name )
	{
		auto it = mData.find( name );

		if( it != mData.end() )
		{
			delete it->second;
			mData.erase( it );
		}
	}

	Shader* ShaderManager::Get( const std::string& name )
	{
		auto it = mData.find( name );

		if( it != mData.end() )
			return it->second;

		return NULL;
	}

	void ShaderManager::GetAll( std::vector< std::string >& names, std::vector< const Shader* >& shaders )
	{
		shaders.clear();
		shaders.reserve( mData.size() );
		
		names.clear();
		names.reserve( mData.size() );
		
		for( auto kv = mData.begin(); kv != mData.end(); ++kv )
		{
			names.push_back( kv->first );
			shaders.push_back( kv->second );
		} 
	}
}
