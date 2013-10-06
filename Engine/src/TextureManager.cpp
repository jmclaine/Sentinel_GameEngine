#include "TextureManager.h"
#include "Renderer.h"
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
			BYTE* pixels = (BYTE*)Renderer::Inst()->GetTexturePixels( texture );

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

			ULONG bound = (width << 2) * height;
			BYTE* pixels = new BYTE[ bound ];

			uncompress( pixels, &bound, comp_pixels, (ULONG)size );

			// Create the texture resource.
			//
			std::shared_ptr< Texture > texture = Renderer::Inst()->CreateTextureFromMemory( pixels, width, height, IMAGE_FORMAT_RGBA );
			
			delete[] comp_pixels;
			delete[] pixels;

			if( !Add( name, texture ))
				throw std::exception( "Failed to load texture." );
		}
	}
}
