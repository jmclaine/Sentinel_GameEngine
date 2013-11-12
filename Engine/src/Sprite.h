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

		std::vector< Quad >			mFrameCoords;

	public:

		std::shared_ptr< Texture >	mTexture;
		std::shared_ptr< Shader >	mShader;

		//////////////////////////////////////

		Sprite( std::shared_ptr< Shader > shader, std::shared_ptr< Texture > texture );
		~Sprite();

		//////////////////////////////////////

		// Set frame coordinates as the pixel position within texture,
		// i.e. AddFrame( Rect( 0, 0, 64, 64 ));
		//
		void	AddFrame( const Quad& coords );
		void	RemoveFrame( UINT index );
		Quad&	GetFrame( UINT index );
		UINT	NumFrames();

		Quad	GetTextureCoords( const Quad& coords );
	};
}
