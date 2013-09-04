#include "Model.h"

namespace Sentinel
{
	Shader*	Model::SHADER_COLOR			= NULL;
	Shader*	Model::SHADER_TEXTURE		= NULL;
	Shader*	Model::SHADER_NORMAL_MAP	= NULL;
	Shader*	Model::SHADER_PARALLAX		= NULL;
	Shader*	Model::SHADER_SKINNING		= NULL;

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

	void Model::SetShaderColor( Shader* shader )
	{
		SHADER_COLOR = shader;
	}

	Shader*& Model::GetShaderColor()
	{
		return SHADER_COLOR;
	}

	void Model::SetShaderTexture( Shader* shader )
	{
		SHADER_TEXTURE = shader;
	}

	Shader*& Model::GetShaderTexture()
	{
		return SHADER_TEXTURE;
	}
	
	void Model::SetShaderNormalMap( Shader* shader )
	{
		SHADER_NORMAL_MAP = shader;
	}

	Shader*& Model::GetShaderNormalMap()
	{
		return SHADER_NORMAL_MAP;
	}
	
	void Model::SetShaderParallax( Shader* shader )
	{
		SHADER_PARALLAX = shader;
	}

	Shader*& Model::GetShaderParallax()
	{
		return SHADER_PARALLAX;
	}
	
	void Model::SetShaderSkinning( Shader* shader )
	{
		SHADER_SKINNING = shader;
	}

	Shader*& Model::GetShaderSkinning()
	{
		return SHADER_SKINNING;
	}
}