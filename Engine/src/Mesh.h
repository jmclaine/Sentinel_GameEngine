#pragma once

// Mesh is not modifiable, except the buffers
// associated with the Mesh.
//
// Mesh is generated through MeshBuilder.h or Model.h
//
#include "Renderer.h"
#include "Matrix4f.h"
#include "Vector4f.h"

namespace Sentinel
{
	enum TextureType
	{
		TEXTURE_DIFFUSE,
		TEXTURE_NORMAL,
		TEXTURE_DEPTH,

		NUM_TEXTURES,
	};

	class Mesh
	{
		friend class MeshBuilder;

	private:

		PrimitiveType		mPrimitive;
		
		Matrix4f			mMatrixWorld;
		Matrix4f			mMatrixShadow;		// camera matrix
		Vector4f			mTextureScale;

		Shader*				mShader;
		
		Material			mMaterial;
		Texture*			mTexture[ NUM_TEXTURES ];

		Buffer*				mVBO;
		Buffer*				mIBO;

	public:

		Mesh();
		~Mesh();

		void		SetWorldTransform( const Matrix4f& world );
		void		SetShadowTransform( const Matrix4f& shadow );
		void		SetTextureScale( const Vector4f& scale );

		void		SetShader( Shader* shader );
		void		SetMaterial( const Material& material );
		void		SetTexture( Texture* texture, TextureType type = TEXTURE_DIFFUSE );

		Buffer*		GetVBO();
		Buffer*		GetIBO();

		void		Draw( UINT count = UINT_MAX );
	};
}
