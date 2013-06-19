#pragma once

#include "Renderer.h"

namespace Sentinel
{
	enum TextureType
	{
		TEXTURE_DIFFUSE,
		TEXTURE_NORMAL,
		TEXTURE_DEPTH,

		NUM_TEXTURES,
	};

	class MeshBuilder;

	class Mesh
	{
		UINT				mNumVertices;
		UINT				mNumIndices;

	public:

		PrimitiveType		mPrimitive;
		
		mat4f				mMatrixWorld;
		mat4f				mMatrixShadow;		// camera matrix
		vec4f				mTextureScale;

		Shader*				mShader;
		
		Material			mMaterial;
		Texture*			mTexture[ NUM_TEXTURES ];

		Buffer*				mVBO;
		Buffer*				mIBO;

		//////////////////////////////////////////

		Mesh();
		~Mesh();

		UINT NumVertices();
		UINT NumIndices();

		void Draw( UINT count = UINT_MAX );

		//////////////////////////////////////////

		friend class MeshBuilder;
	};
}
