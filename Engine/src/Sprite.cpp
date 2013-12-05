#include "Sprite.h"
#include "Texture.h"
#include "Vector2f.h"
#include "Point.h"

namespace Sentinel
{
	Sprite::Sprite()
	{}

	Sprite::Sprite( std::shared_ptr< Texture > texture ) :
		mTexture( texture )
	{}

	Sprite::~Sprite()
	{}

	//////////////////////////////////////

	void Sprite::AddFrame( const Quad& coords )
	{
		mFrame.push_back( coords );
	}

	void Sprite::RemoveFrame( UINT index )
	{
		mFrame.erase( mFrame.begin() + index );
	}

	Quad& Sprite::GetFrame( UINT index )
	{
		return mFrame[ index ];
	}

	UINT Sprite::NumFrames()
	{
		return mFrame.size();
	}

	Quad Sprite::GetTextureCoords( const Quad& coords )
	{
		return Quad( coords.left   / static_cast< float >(mTexture->Width()),
					 coords.top    / static_cast< float >(mTexture->Height()),
					 coords.right  / static_cast< float >(mTexture->Width()), 
					 coords.bottom / static_cast< float >(mTexture->Height()) );
	}
}
