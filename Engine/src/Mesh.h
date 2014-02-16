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
	class MaterialManager;
	class VertexLayout;
	class Material;

	class SENTINEL_DLL Mesh
	{
		friend class MeshBuilder;

	public:

		RenderType			mPrimitive;
		
		Buffer*				mVBO;
		Buffer*				mIBO;

		std::shared_ptr< VertexLayout >		mLayout;
		std::shared_ptr< Material >			mMaterial;

		Matrix4f			mMatrixWorld;
		
		BoundingBox			mBounds;

		//////////////////////////////

		Mesh();
		~Mesh();

		void		 Draw( Renderer* renderer, GameWorld* world, UINT count = UINT_MAX );

		// Helper function to calculate the bounding box using the VBO.
		//
		void		 CalculateBounds();

		//////////////////////////////

		static void  Save( Archive&				archive, 
						   Mesh*				mesh,
						   Renderer*			renderer, 
						   ShaderManager*		shaderManager, 
						   TextureManager*		textureManager,
						   MaterialManager*		materialManager );

		static Mesh* Load( Archive&				archive, 
						   Renderer*			renderer, 
						   ShaderManager*		shaderManager, 
						   TextureManager*		textureManager,
						   MaterialManager*		materialManager );
	};
}
