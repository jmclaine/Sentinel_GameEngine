#include "SpriteManager.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "Archive.h"

namespace Sentinel
{
	SpriteManager::SpriteManager()
	{}

	SpriteManager::~SpriteManager()
	{}

	void SpriteManager::Save( Archive&			archive, 
							  TextureManager*	textureManager )
	{
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			archive.Write( &it->first );

			std::shared_ptr< Sprite > sprite = it->second;

			archive.Write( &textureManager->Get( sprite->mTexture ));

			UINT size = sprite->NumFrames();
			archive.Write( &size );

			for( UINT x = 0; x < size; ++x )
			{
				archive.Write( (BYTE*)&sprite->GetFrame( x ), sizeof( Quad ));
			}
		}
	}

	void SpriteManager::Load( Archive&			archive, 
							  TextureManager*	textureManager )
	{
		RemoveAll();

		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::string name;
			archive.Read( &name );

			std::string texture;
			archive.Read( &texture );

			std::shared_ptr< Sprite > sprite( new Sprite( textureManager->Get( texture )));

			UINT size;
			archive.Read( &size );

			Quad q;
			for( UINT x = 0; x < size; ++x )
			{
				archive.Read( (BYTE*)&q, sizeof( Quad ));

				sprite->AddFrame( q );
			}

			if( !Add( name, sprite ))
				throw std::exception( "Failed to load Sprite." );
		}
	}
}
