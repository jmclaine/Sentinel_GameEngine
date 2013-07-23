#include "Model.h"

namespace Sentinel
{
	Shader*	Model::SHADER_COLOR			= NULL;
	Shader*	Model::SHADER_TEXTURE		= NULL;
	Shader*	Model::SHADER_NORMAL_MAP	= NULL;
	Shader*	Model::SHADER_PARALLAX		= NULL;
	Shader*	Model::SHADER_SKINNING		= NULL;

	Model* Model::Load( const char* filename )
	{
		// Determine the extension of the object,
		// and load it according to its type.
		//
		int len = strlen( filename ) - 1;
		if( toupper(filename[ len - 2 ]) == 'O' && toupper(filename[ len - 1 ]) == 'B' && toupper(filename[ len ]) == 'J' )
		{
			//return LoadModelOBJ( filename );
		}
		else
		if( toupper(filename[ len - 2 ]) == 'M' && toupper(filename[ len - 1 ]) == '3' && toupper(filename[ len ]) == 'D' )
		{
			return LoadModelM3D( filename );
		}

		return NULL;
	}

	void Model::SetWorldTransform( const Matrix4f& world )
	{
		mMatrixWorld = world;
	}

	const Matrix4f& Model::GetWorldTransform() const
	{
		return mMatrixWorld;
	}
}