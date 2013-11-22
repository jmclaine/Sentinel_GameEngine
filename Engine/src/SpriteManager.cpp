#include "SpriteManager.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Archive.h"

namespace Sentinel
{
	SpriteManager::SpriteManager()
	{}

	SpriteManager::~SpriteManager()
	{}

	void SpriteManager::Save( Archive&			archive, 
							  ShaderManager*	shaderManager, 
							  TextureManager*	textureManager )
	{
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			archive.Write( &it->first );

			std::shared_ptr< Sprite > sprite = it->second;

			std::string str;
			
			str = shaderManager->Get( sprite->mShader );
			archive.Write( &str );

			str = textureManager->Get( sprite->mTexture );
			archive.Write( &str );

			UINT size = sprite->NumFrames();
			archive.Write( &size );

			for( UINT x = 0; x < size; ++x )
			{
				archive.Write( (BYTE*)&sprite->GetFrame( x ), sizeof( Quad ));
			}
		}
	}

	void SpriteManager::Load( Archive&			archive, 
							  ShaderManager*	shaderManager, 
							  TextureManager*	textureManager )
	{
		RemoveAll();

		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::string name;
			archive.Read( &name );

			std::string shader;
			archive.Read( &shader );

			std::string texture;
			archive.Read( &texture );

			std::shared_ptr< Sprite > sprite( new Sprite( shaderManager->Get( shader ), textureManager->Get( texture )));

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
