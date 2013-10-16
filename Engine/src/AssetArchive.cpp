#include "Archive.h"
#include "AssetArchive.h"
#include "Renderer.h"
#include "Buffer.h"
#include "Mesh.h"
#include "zlib.h"
#include "ShaderManager.h"
#include "TextureManager.h"

typedef unsigned long ULONG;

namespace Sentinel
{
	void AssetArchive::SaveBuffer( Archive& archive, Buffer* buffer )
	{
		BYTE type = (BYTE)buffer->Type();
		archive.Write( &type );

		UINT size = buffer->Size();
		archive.Write( &size );

		UINT stride = buffer->Stride();
		archive.Write( &stride );
		
		BYTE* data = (BYTE*)buffer->Lock();

		ULONG bound = compressBound( size );
		BYTE* comp_data = (BYTE*)malloc( bound );

		compress( comp_data, &bound, data, size );

		archive.Write( &bound );
		archive.Write( comp_data, bound );

		buffer->Unlock();

		free( comp_data );
	}

	Buffer* AssetArchive::LoadBuffer( Archive& archive, Renderer* renderer )
	{
		BYTE type;
		archive.Read( &type );
		
		UINT size;
		archive.Read( &size );

		UINT stride;
		archive.Read( &stride );

		ULONG bound;
		archive.Read( &bound );

		BYTE* comp_data = (BYTE*)malloc( bound );
		archive.Read( comp_data, bound );

		BYTE*  data = (BYTE*)malloc( size );
		ULONG uSize = (ULONG)size;
		uncompress( data, (ULONG*)(&size), comp_data, bound );

		Buffer* buffer = renderer->CreateBuffer( data, size, stride, (BufferType)type );

		free( comp_data );
		free( data );

		return buffer;
	}

	void AssetArchive::SaveMesh( Archive&			archive, 
								 Mesh*				mesh,
								 Renderer*			renderer, 
							     ShaderManager*		shaderManager, 
							     TextureManager*	textureManager )
	{
		BYTE primitive = mesh->mPrimitive;
		archive.Write( &primitive );

		SaveBuffer( archive, mesh->mVBO );
		SaveBuffer( archive, mesh->mIBO );

		archive.Write( &shaderManager->Get( mesh->mShader ));

		archive.Write( (BYTE*)mesh->mMaterial.Ptr(), sizeof( Material ));

		for( UINT x = 0; x < NUM_TEXTURES; ++x )
		{
			BYTE type;
			if( mesh->mTexture[ x ].get() == renderer->NULL_TEXTURE.get() )
				type = 0;
			else
			if( mesh->mTexture[ x ].get() == renderer->BASE_TEXTURE.get() )
				type = 1;
			else
				type = 2;

			archive.Write( &type );

			if( type == 2 )
				archive.Write( &textureManager->Get( mesh->mTexture[ x ] ));
		}
	}

	Mesh* AssetArchive::LoadMesh( Archive&			archive, 
								  Renderer*			renderer, 
							      ShaderManager*	shaderManager, 
							      TextureManager*	textureManager )
	{
		Mesh* mesh = new Mesh();

		BYTE primitive;
		archive.Read( &primitive );
		mesh->mPrimitive = (PrimitiveType)primitive;

		mesh->mVBO = LoadBuffer( archive, renderer );
		mesh->mIBO = LoadBuffer( archive, renderer );

		std::string shader;
		archive.Read( &shader );
		mesh->mShader = shaderManager->Get( shader );

		archive.Read( (BYTE*)mesh->mMaterial.Ptr(), sizeof( Material ));
		
		std::string texture;
		for( UINT x = 0; x < NUM_TEXTURES; ++x )
		{
			BYTE type;
			archive.Read( &type );

			if( type == 0 )
				mesh->mTexture[ x ] = renderer->NULL_TEXTURE;
			else
			if( type == 1 )
				mesh->mTexture[ x ] = renderer->BASE_TEXTURE;
			else
			{
				std::string texture;
				archive.Read( &texture );

				mesh->mTexture[ x ] = textureManager->Get( texture );
			}
		}

		return mesh;
	}
}
