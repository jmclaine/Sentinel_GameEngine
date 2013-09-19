#include "Util.h"
#include "Shader.h"
#include "ShaderManager.h"

namespace Sentinel
{
	Shader::Shader()
	{
		mVertexSize = 0;
	}

	Shader::~Shader()
	{}

	const std::string& Shader::AttributeDecl()
	{
		return mAttribute;
	}

	const std::string& Shader::UniformDecl()
	{
		return mUniform;
	}

	UINT Shader::VertexSize()
	{
		return mVertexSize;
	}

	void Shader::ProcessUniforms()
	{
		char name[ 32 ];
		UINT lightCount = 0;
		UINT texCount = 0;
		for( UINT i = 0; i < mUniform.size(); ++i )
		{
			switch( mUniform[ i ] )
			{
				case 'P':
					CreateUniform( "wvp" );
					break;

				case 'p':
					CreateUniform( "world" );
					break;

				case 'X':
					sprintf_s( name, "tex%d", texCount );
					CreateUniform( name );
					++texCount;
					break;

				case 'x':
					CreateUniform( "texScale" );
					break;

				case 'V':
					CreateUniform( "cam_pos" );
					break;

				case 'L':
					sprintf_s( name, "light_pos%d", lightCount );
					CreateUniform( name );
					sprintf_s( name, "light_color%d", lightCount );
					CreateUniform( name );
					sprintf_s( name, "light_attn%d", lightCount );
					CreateUniform( name );
					++lightCount;
					break;

				case 'M':
					CreateUniform( "ambient" );
					CreateUniform( "diffuse" );
					CreateUniform( "specular" );
					CreateUniform( "spec_comp" );
					break;

				case 'S':
					CreateUniform( "mlvlp" );
					CreateUniform( "shadow_epsilon" );
					break;

				case 's':
					CreateUniform( "rtScale" );
					break;

				case 'B':
					CreateUniform( "bone" );
					break;

				case 't':
					CreateUniform( "deltaTime" );
					break;

				default:
					TRACE( mUniform[ i ] << " = UNIFORM UNKNOWN" );
					break;
			}
		}
	}
}