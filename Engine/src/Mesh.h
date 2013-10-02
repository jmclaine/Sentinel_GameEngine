#pragma once
/*
Mesh is fully customizable.

Mesh creation helpers are in MeshBuilder.h or Model.h
*/
#include "Common.h"
#include "Renderer.h"
#include "Material.h"
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

	class SENTINEL_DLL Mesh
	{
		friend class MeshBuilder;

	public:

		PrimitiveType		mPrimitive;
		
		Buffer*				mVBO;
		Buffer*				mIBO;

		Matrix4f			mMatrixWorld;
		Matrix4f			mMatrixShadow;		// camera matrix
		Vector4f			mTextureScale;

		std::shared_ptr< Shader >	mShader;
		
		Material			mMaterial;
		std::shared_ptr< Texture >	mTexture[ NUM_TEXTURES ];

		//////////////////////////////

		Mesh();
		~Mesh();

		void Draw( UINT count = UINT_MAX );
	};
}
