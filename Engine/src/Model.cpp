#include "Model.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Archive.h"

namespace Sentinel
{
	Model::~Model()
	{ }

	Model* Model::Load(
		const char* filename,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager)
	{
		// Determine the extension of the object,
		// and load it according to its type.
		//
		int len = strlen(filename) - 1;
		if (toupper(filename[len - 2]) == 'O' && toupper(filename[len - 1]) == 'B' && toupper(filename[len]) == 'J')
		{
			return LoadModelOBJFromFile(filename, renderer, shaderManager, textureManager, materialManager, blendManager);
		}
		else if (toupper(filename[len - 2]) == 'M' && toupper(filename[len - 1]) == '3' && toupper(filename[len]) == 'D')
		{
			return LoadModelM3DFromFile(filename, renderer, shaderManager, textureManager, materialManager, blendManager);
		}

		return nullptr;
	}

	Model* Model::Load(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager)
	{
		BYTE format;
		archive.Read(&format);

		if (format == Model::OBJ)
		{
			return LoadModelOBJFromArchive(
				archive, 
				renderer,
				shaderManager, 
				textureManager, 
				materialManager, 
				blendManager);
		}
		else if (format == Model::M3D)
		{
			return LoadModelM3DFromArchive(
				archive, 
				renderer, 
				shaderManager, 
				textureManager, 
				materialManager, 
				blendManager);
		}

		return nullptr;
	}
}
