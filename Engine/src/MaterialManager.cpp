#include "MaterialManager.h"
#include "Archive.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace Sentinel
{
	MaterialManager::MaterialManager()
	{}

	MaterialManager::~MaterialManager()
	{}

	/////////////////////////////////

	void MaterialManager::Save( Archive&			archive, 
								ShaderManager*		shaderManager, 
								TextureManager*		textureManager )
	{}

	void MaterialManager::Load( Archive&			archive, 
								ShaderManager*		shaderManager, 
								TextureManager*		textureManager )
	{}
}
