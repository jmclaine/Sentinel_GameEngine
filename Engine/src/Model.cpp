#include "Model.h"

namespace Sentinel
{
	std::shared_ptr< Shader > Model::SHADER_COLOR		= NULL;
	std::shared_ptr< Shader > Model::SHADER_TEXTURE		= NULL;
	std::shared_ptr< Shader > Model::SHADER_NORMAL_MAP	= NULL;
	std::shared_ptr< Shader > Model::SHADER_PARALLAX	= NULL;
	std::shared_ptr< Shader > Model::SHADER_SKINNING	= NULL;

	Model::~Model()
	{}

	Model* Model::Load( const char* filename )
	{
		// Determine the extension of the object,
		// and load it according to its type.
		//
		int len = strlen( filename ) - 1;
		if( toupper(filename[ len - 2 ]) == 'O' && toupper(filename[ len - 1 ]) == 'B' && toupper(filename[ len ]) == 'J' )
		{
			return LoadModelOBJ( filename );
		}
		else
		if( toupper(filename[ len - 2 ]) == 'M' && toupper(filename[ len - 1 ]) == '3' && toupper(filename[ len ]) == 'D' )
		{
			return LoadModelM3D( filename );
		}

		return NULL;
	}

	void Model::SetShaderColor( const std::shared_ptr< Shader >& shader )
	{
		SHADER_COLOR = shader;
	}

	std::shared_ptr< Shader >& Model::GetShaderColor()
	{
		return SHADER_COLOR;
	}

	void Model::SetShaderTexture( const std::shared_ptr< Shader >& shader )
	{
		SHADER_TEXTURE = shader;
	}

	std::shared_ptr< Shader >& Model::GetShaderTexture()
	{
		return SHADER_TEXTURE;
	}
	
	void Model::SetShaderNormalMap( const std::shared_ptr< Shader >& shader )
	{
		SHADER_NORMAL_MAP = shader;
	}

	std::shared_ptr< Shader >& Model::GetShaderNormalMap()
	{
		return SHADER_NORMAL_MAP;
	}
	
	void Model::SetShaderParallax( const std::shared_ptr< Shader >& shader )
	{
		SHADER_PARALLAX = shader;
	}

	std::shared_ptr< Shader >& Model::GetShaderParallax()
	{
		return SHADER_PARALLAX;
	}
	
	void Model::SetShaderSkinning( const std::shared_ptr< Shader >& shader )
	{
		SHADER_SKINNING = shader;
	}

	std::shared_ptr< Shader >& Model::GetShaderSkinning()
	{
		return SHADER_SKINNING;
	}
}