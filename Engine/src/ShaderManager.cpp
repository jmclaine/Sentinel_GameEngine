#include "ShaderManager.h"
#include "Renderer.h"
#include "tinyxml.h"
#include "zlib.h"

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
	{
		// Store the number of shaders.
		//
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			std::shared_ptr< Shader > shader = it->second;

			// Store shader info.
			//
			archive.Write( &it->first );

			archive.Write( &shader->Attribute() );
			archive.Write( &shader->Uniform() );

			// Store the compressed shader source.
			//
			ULONG  size  = (ULONG)strlen( shader->Source() );
			ULONG bound  = compressBound( size );

			BYTE* comp_source = new BYTE[ bound ];

			compress( comp_source, &bound, reinterpret_cast< const Bytef* >(shader->Source()), size );

			archive.Write( &size, 1, true );
			archive.Write( &bound, 1, true );
			archive.Write( comp_source, bound );

			delete[] comp_source;
		}
	}

	void ShaderManager::Load( Archive& archive )
	{
		// Read the number of shaders to load.
		//
		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::string name;
			archive.Read( &name );

			std::string attrib;
			archive.Read( &attrib );

			std::string uniform;
			archive.Read( &uniform );

			ULONG size;
			archive.Read( &size, 1, true );

			ULONG bound;
			archive.Read( &bound, 1, true );

			char* comp_source = (char*)malloc( bound );
			archive.Read( comp_source, bound );

			char* source = (char*)malloc( size+1 );
			source[ size ] = 0;

			uncompress( reinterpret_cast< Bytef* >(source), &size, reinterpret_cast< Bytef* >(comp_source), bound );

			if( !Add( name, Renderer::Inst()->CreateShaderFromMemory( source, attrib.c_str(), uniform.c_str() )))
				throw std::exception( "Failed to read shader." );
		}
	}

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

			std::shared_ptr< Shader > shader = ShaderManager::Inst()->Add( pName, Renderer::Inst()->CreateShaderFromFile( pFile, pAttr, pUnif ));
			
			if( !shader )
				return false;

			pElem = pElem->NextSiblingElement();
		}

		return true;
	}
}
