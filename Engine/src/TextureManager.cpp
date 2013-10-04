#include "TextureManager.h"
#include "Renderer.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

namespace Sentinel
{
	TextureManager::TextureManager()
	{}

	TextureManager::~TextureManager()
	{}

	TextureManager* TextureManager::Create()
	{
		return TextureManager::Inst();
	}

	void TextureManager::Save( Archive& archive )
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

			UINT width = texture->Width();
			archive.Write( &width );

			UINT height = texture->Height();
			archive.Write( &height );

			// Store the texture file in place.
			//
			BYTE* pixels = (BYTE*)texture->GetPixels();
			UINT  size   = (width * height) << 4; // RGBA; 32-bit color

			ULONG bound  = compressBound( size );
			BYTE* comp_pixels = new BYTE[ bound ];

			compress( comp_pixels, &bound, pixels, size );

			archive.Write( &bound, 1, true );
			archive.Write( comp_pixels, bound );

			delete[] comp_pixels;
			delete[] pixels;
		}
	}

	void TextureManager::Load( Archive& archive )
	{
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

			UINT width;
			archive.Read( &width );

			UINT height;
			archive.Read( &height );

			// Read the texture data.
			//
			UINT size;
			archive.Read( &size, 1, true );

			BYTE* comp_pixels = new BYTE[ size ];
			archive.Read( comp_pixels, size );

			ULONG bound = (width * height) << 4;
			BYTE* pixels = new BYTE[ bound ];

			uncompress( pixels, &bound, comp_pixels, (ULONG)size );

			// Create the texture resource.
			//
			std::shared_ptr< Texture > texture = Renderer::Inst()->CreateTextureFromMemory( pixels, width, height, IMAGE_FORMAT_RGBA );
			Add( name, texture );

			delete[] comp_pixels;
			delete[] pixels;
		}
	}
}
