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
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			std::shared_ptr< Shader > shader = it->second;

			// Store shader info.
			//
			archive.Write( &it->first );

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

		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::string name;
			archive.Read( &name );

			// Uncompress shader source.
			//
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

			if( !Add( name, SHARED( renderer->CreateShaderFromMemory( source ))))
				throw std::exception( "Failed to read shader." );

			free( comp_source );
		}
	}
}
