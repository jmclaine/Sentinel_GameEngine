#include "MeshManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace Sentinel
{
	MeshManager::MeshManager()
	{}

	MeshManager::~MeshManager()
	{}

	MeshManager* MeshManager::Create()
	{
		return MeshManager::Inst();
	}

	void MeshManager::Save( Archive& archive )
	{
		UINT count = mData.size();
		archive.Write( &count );

		TRAVERSE_LIST( it, mData )
		{
			archive.Write( &it->first );

			SaveMesh( archive, it->second );
		}
	}

	void MeshManager::Load( Archive& archive )
	{
		UINT count;
		archive.Read( &count );

		for( UINT x = 0; x < count; ++x )
		{
			std::shared_ptr< Mesh > mesh( new Mesh() );

			std::string name;
			archive.Read( &name );

			LoadMesh( archive, mesh );

			if( !Add( name, mesh ))
				throw std::exception( "Failed to load mesh." );
		}
	}

	void MeshManager::SaveMesh( Archive& archive, std::shared_ptr< Mesh > mesh )
	{
		BYTE primitive = mesh->mPrimitive;
		archive.Write( &primitive );

		mesh->mVBO->Save( archive );
		mesh->mIBO->Save( archive );

		archive.Write( &ShaderManager::Inst()->Get( mesh->mShader ));

		archive.Write( mesh->mMaterial.Ptr(), sizeof( Material ));

		for( UINT x = 0; x < NUM_TEXTURES; ++x )
		{
			BYTE type;
			if( mesh->mTexture[ x ].get() == Renderer::Inst()->NULL_TEXTURE.get() )
				type = 0;
			else
			if( mesh->mTexture[ x ].get() == Renderer::Inst()->BASE_TEXTURE.get() )
				type = 1;
			else
				type = 2;

			archive.Write( &type );

			if( type == 2)
				archive.Write( &TextureManager::Inst()->Get( mesh->mTexture[ x ] ));
		}
	}

	void MeshManager::LoadMesh( Archive& archive, std::shared_ptr< Mesh > mesh )
	{
		BYTE primitive;
		archive.Read( &primitive );
		mesh->mPrimitive = (PrimitiveType)primitive;

		mesh->mVBO = Buffer::Load( archive );
		mesh->mIBO = Buffer::Load( archive );

		std::string shader;
		archive.Read( &shader );
		mesh->mShader = ShaderManager::Inst()->Get( shader );

		archive.Read( mesh->mMaterial.Ptr(), sizeof( Material ));

		std::string texture;
		for( UINT x = 0; x < NUM_TEXTURES; ++x )
		{
			BYTE type;
			archive.Read( &type );

			if( type == 0 )
				mesh->mTexture[ x ] = Renderer::Inst()->NULL_TEXTURE;
			else
			if( type == 1 )
				mesh->mTexture[ x ] = Renderer::Inst()->BASE_TEXTURE;
			else
			{
				std::string texture;
				archive.Read( &texture );
				mesh->mTexture[ x ] = TextureManager::Inst()->Get( texture );
			}
		}
	}
}
