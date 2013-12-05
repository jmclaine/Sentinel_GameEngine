#pragma once

#include <memory>
#include <vector>

#include "Common.h"
#include "Point.h"

namespace Sentinel
{
	class Texture;
	class Shader;
	
	class SENTINEL_DLL Sprite
	{
	protected:

		std::vector< Quad >			mFrame;

	public:

		std::shared_ptr< Texture >	mTexture;

		//////////////////////////////////////

		Sprite();
		Sprite( std::shared_ptr< Texture > texture );
		~Sprite();

		//////////////////////////////////////

		// Set frame texture coordinates.
		// Use GetTextureCoords() for easy conversion.
		//
		void	AddFrame( const Quad& coords );
		void	RemoveFrame( UINT index );
		Quad&	GetFrame( UINT index );
		UINT	NumFrames();
		
		// Convert pixel coordinates to texture coordinates.
		//
		Quad	GetTextureCoords( const Quad& coords );
	};
}
