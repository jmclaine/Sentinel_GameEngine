#include "ShaderManager.h"
#include "Shader.h"
#include "Archive.h"
#include "Renderer.h"
#include "tinyxml.h"
#include "zlib.h"

namespace Sentinel
{
	ShaderManager::ShaderManager()
	{}

	ShaderManager::~ShaderManager()
	{}

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

			//archive.Write( &shader->Attribute() );
			//archive.Write( &shader->Uniform() );

			// Store the compressed shader source.
			//
			char* source = (char*)shader->Source();
			ULONG  size  = (ULONG)strlen( source );
			ULONG bound  = compressBound( size );

			BYTE* comp_source = (BYTE*)malloc( bound );

			compress( comp_source, &bound, reinterpret_cast< const Bytef* >(source), size );

			archive.Write( &size, 1, true );
			archive.Write( &bound, 1, true );
			archive.Write( comp_source, bound );

			free( comp_source );
		}
	}

	void ShaderManager::Load( Archive& archive, Renderer* renderer )
	{
		RemoveAll();

		// Read the number of shaders to load.
		//
		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::string name;
			archive.Read( &name );

			//std::string attrib;
			//archive.Read( &attrib );

			//std::string uniform;
			//archive.Read( &uniform );

			ULONG size;
			archive.Read( &size, 1, true );

			ULONG bound;
			archive.Read( &bound, 1, true );

			char* comp_source = (char*)malloc( bound );
			archive.Read( comp_source, bound );

			char* source = (char*)malloc( size + 1 );
			source[ size ] = 0;

			uncompress( reinterpret_cast< Bytef* >(source), &size, reinterpret_cast< Bytef* >(comp_source), bound );

			TRACE( "Compiling '" << name << "'..." );

			//if( !Add( name, renderer->CreateShaderFromMemory( source, attrib, uniform )))
			//	throw std::exception( "Failed to read shader." );

			free( comp_source );
		}
	}

	bool ShaderManager::LoadConfig( const char* filename, Renderer* renderer, ShaderManager* shaderManager )
	{
		TiXmlDocument doc;
		if( !doc.LoadFile( filename ))
		{
			TRACE( "Failed to open '" << filename << "'" );
			return false;
		}

		TiXmlHandle	hDoc( &doc );

		// Starting enclosure: <Shaders>
		//
		TiXmlElement* pMain = hDoc.FirstChild( "Shaders" ).Element();

		// Read <Definition>
		//
		TiXmlElement* pDef = pMain->FirstChild( "Definition" )->ToElement();

		// Store each shader.
		//
		while( pDef != NULL )
		{
			const char* pFile = pDef->Attribute( "FileName" );
			const char* pName = pDef->Attribute( "Name" );

			std::shared_ptr< Shader > shader = shaderManager->Add( pName, renderer->CreateShaderFromFile( pFile ));

			if( !shader )
			{
				TRACE( "Failed to load shader '" << std::string( pFile ) << "'" );
				return false;
			}

			pDef = pDef->NextSiblingElement();
		}

		return true;
	}
}
