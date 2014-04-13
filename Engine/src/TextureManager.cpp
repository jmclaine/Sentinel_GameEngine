#include "TextureManager.h"
#include "Archive.h"
#include "Renderer.h"
#include "Texture.h"
#include "zlib.h"

namespace Sentinel
{
	TextureManager::TextureManager( Renderer* renderer )
	{
		Add( "NULL_TEXTURE", renderer->NULL_TEXTURE );
		Add( "BASE_TEXTURE", renderer->BASE_TEXTURE );
	}

	TextureManager::~TextureManager()
	{}

	void TextureManager::Save( Archive& archive, Renderer* renderer )
	{
		// Store the number of textures.
		//
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			std::shared_ptr< Texture > texture = it->second;

			// Store texture info.
			//
			archive.Write( &it->first );

			short width = texture->Width();
			archive.Write( &width );

			short height = texture->Height();
			archive.Write( &height );

			// Store the texture file in place.
			//
			BYTE* pixels = (BYTE*)renderer->GetTexturePixels( texture.get() );

			if( !pixels )
				throw std::exception( "Failed to GetTexturePixels." );

			UINT  size   = (width << 2) * height; // RGBA; 32-bit color
			ULONG bound  = compressBound( size );

			BYTE* comp_pixels = new BYTE[ bound ];

			compress( comp_pixels, &bound, pixels, size );

			archive.Write( &bound, 1, true );
			archive.Write( comp_pixels, bound );

			delete[] comp_pixels;
			delete[] pixels;
		}
	}

	void TextureManager::Load( Archive& archive, Renderer* renderer )
	{
		RemoveAll();

		// Read the number of textures to load.
		//
		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			// Read texture info.
			//
			std::string name;
			archive.Read( &name );

			short width;
			archive.Read( &width );

			short height;
			archive.Read( &height );

			// Read the texture data.
			//
			UINT size;
			archive.Read( &size );

			BYTE* comp_pixels = new BYTE[ size ];
			archive.Read( comp_pixels, size );

			ULONG bound = (width << 2) * height;
			BYTE* pixels = new BYTE[ bound ];

			uncompress( pixels, &bound, comp_pixels, (ULONG)size );

			// Create the texture resource.
			//
			std::shared_ptr< Texture > texture( renderer->CreateTextureFromMemory( pixels, width, height, ImageFormat::RGBA ));
			
			delete[] comp_pixels;
			delete[] pixels;

			if( !Add( name, texture ))
				throw std::exception( "Failed to load texture." );
		}
	}
}
