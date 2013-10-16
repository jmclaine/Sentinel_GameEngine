#pragma once
/*
Saves and Loads assets.
*/
#include "Archive.h"

namespace Sentinel
{
	class Buffer;
	class Mesh;
	class Renderer;
	class ShaderManager;
	class TextureManager;

	class AssetArchive
	{
	public:

		static void		SaveBuffer( Archive& archive, Buffer* buffer );
		static Buffer*  LoadBuffer( Archive& archive, Renderer* renderer );

		///////////////////////////////////////////////////////////////////////

		static void		SaveMesh( Archive&			archive, 
								  Mesh*				mesh,
								  Renderer*			renderer, 
								  ShaderManager*	shaderManager, 
								  TextureManager*	textureManager );

		static Mesh*	LoadMesh( Archive&			archive, 
								  Renderer*			renderer, 
								  ShaderManager*	shaderManager, 
								  TextureManager*	textureManager );
	};
}
