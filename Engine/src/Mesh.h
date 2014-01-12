#pragma once
/*
Mesh is fully customizable.

Mesh creation helpers are in MeshBuilder.h or Model.h
*/
#include <memory>

#include "Common.h"
#include "RendererTypes.h"
#include "Matrix4f.h"
#include "Vector4f.h"
#include "Material.h"
#include "Shape.h"

namespace Sentinel
{
	class Renderer;
	class GameWorld;
	class Buffer;
	class Shader;
	class Texture;
	class Archive;
	class ShaderManager;
	class TextureManager;

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

		BoundingBox			mBounds;

		//////////////////////////////

		Mesh();
		~Mesh();

		void		 Draw( Renderer* renderer, GameWorld* world, UINT count = UINT_MAX );

		// Helper function to calculate the bounding box using the VBO.
		//
		void		 CalculateBounds();

		//////////////////////////////

		static void  Save( Archive&			archive, 
						   Mesh*			mesh,
						   Renderer*		renderer, 
						   ShaderManager*	shaderManager, 
						   TextureManager*	textureManager );

		static Mesh* Load( Archive&			archive, 
						   Renderer*		renderer, 
						   ShaderManager*	shaderManager, 
						   TextureManager*	textureManager );
	};
}
