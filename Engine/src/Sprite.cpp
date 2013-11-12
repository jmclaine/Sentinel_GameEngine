#include "Sprite.h"
#include "Texture.h"
#include "Vector2f.h"
#include "Point.h"

namespace Sentinel
{
	Sprite::Sprite( std::shared_ptr< Shader > shader, std::shared_ptr< Texture > texture ) :
		mShader( shader ),
		mTexture( texture )
	{}

	Sprite::~Sprite()
	{}

	//////////////////////////////////////

	void Sprite::AddFrame( const Quad& coords )
	{
		_ASSERT( mTexture );

		mFrameCoords.push_back( GetTextureCoords( coords ));
	}

	void Sprite::RemoveFrame( UINT index )
	{
		mFrameCoords.erase( mFrameCoords.begin() + index );
	}

	Quad& Sprite::GetFrame( UINT index )
	{
		return mFrameCoords[ index ];
	}

	UINT Sprite::NumFrames()
	{
		return mFrameCoords.size();
	}

	Quad Sprite::GetTextureCoords( const Quad& coords )
	{
		return Quad( coords.left   / static_cast< float >(mTexture->Width()),
					 coords.top    / static_cast< float >(mTexture->Height()),
					 coords.right  / static_cast< float >(mTexture->Width()), 
					 coords.bottom / static_cast< float >(mTexture->Height()) );
	}
}
