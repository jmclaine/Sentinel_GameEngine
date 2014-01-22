#include "ShaderGL.h"
#include "TextureGL.h"
#include "VertexLayoutGL.h"
#include "Archive.h"

namespace Sentinel
{
	ShaderGL::SamplerGL::SamplerGL() :
		mWrapS( GL_REPEAT ),
		mWrapT( GL_REPEAT ),
		mMinFilter( GL_LINEAR ),
		mMagFilter( GL_LINEAR )
	{}

	void ShaderGL::SamplerGL::Create( SamplerMode modeU, SamplerMode modeV, SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter )
	{
		mWrapS = (modeU == MODE_WRAP) ? GL_REPEAT : GL_CLAMP;
		mWrapT = (modeV == MODE_WRAP) ? GL_REPEAT : GL_CLAMP;

		if( minFilter == FILTER_LINEAR )
		{
			if( mipFilter == FILTER_LINEAR )
			{
				mMinFilter = GL_LINEAR_MIPMAP_LINEAR;
			}
			else
			if( mipFilter == FILTER_POINT )
			{
				mMinFilter = GL_LINEAR_MIPMAP_NEAREST;
			}
			else
			{
				mMinFilter = GL_LINEAR;
			}
		}
		else
		{
			if( mipFilter == FILTER_LINEAR )
			{
				mMinFilter = GL_NEAREST_MIPMAP_LINEAR;
			}
			else
			if( mipFilter == FILTER_POINT )
			{
				mMinFilter = GL_NEAREST_MIPMAP_NEAREST;
			}
			else
			{
				mMinFilter = GL_NEAREST;
			}
		}

		if( magFilter == FILTER_LINEAR )
		{
			mMagFilter = GL_LINEAR;
		}
		else
		{
			mMagFilter = GL_NEAREST;
		}
	}

	void ShaderGL::SamplerGL::Apply()
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrapS );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrapT );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mMinFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mMagFilter );
	}

	//////////////////////////////////////////////////////////////////////
		
	ShaderGL::ShaderGL() :
		mProgram( 0 ),
		mVertexShader( 0 ),
		mGeometryShader( 0 ),
		mFragmentShader( 0 ),
		mTextureLevel( 0 ),
		mAttributeSize( 0 )
	{}

	ShaderGL::~ShaderGL()
	{
		Release();
	}

	void ShaderGL::Release()
	{
		if( mSampler )
		{
			for( UINT x = 0; x < mNumSamplers; ++x )
			{
				SAFE_DELETE( mSampler[ x ] );
			}

			SAFE_DELETE_ARRAY( mSampler );
		}
			
		if( mProgram != 0 )
		{
			glDeleteProgram( mProgram );
			mProgram = 0;

			glDeleteShader( mVertexShader );
			mVertexShader = 0;

			glDeleteShader( mGeometryShader );
			mGeometryShader = 0;

			glDeleteShader( mFragmentShader );
			mFragmentShader = 0;
		}
	}

	GLuint ShaderGL::Program()
	{
		return mProgram;
	}

	UINT ShaderGL::AttributeSize()
	{
		return mAttributeSize;
	}

	///////////////////////////////////

	bool ShaderGL::CreateAttribute( char* name, AttributeType type )
	{
		GLint attrib = glGetAttribLocation( mProgram, name );

		mAttributeGL[ type ] = attrib;

		if( attrib != -1 )
		{
			mAttribute.push_back( type );

			++mAttributeSize;

			return true;
		}

		return false;
	}

	bool ShaderGL::CreateUniform( char* name, UniformType type )
	{
		GLint uniform = glGetUniformLocation( mProgram, name );

		if( uniform != -1 )
		{
			mUniform.push_back( type );
			mUniformGL.push_back( uniform );

			return true;
		}

		return false;
	}

	///////////////////////////////////

	UINT ShaderGL::CreateFromFile( std::string filename )
	{
		filename.append( ".xsh" );

		if( Archive::ToBuffer( filename.c_str(), mSource ) == 0 )
		{
			REPORT_ERROR( "Could not open '" << filename << "'", "Shader Loader Error" );

			return S_FALSE;
		}
			
		TRACE( "Compiling '" << filename << "'..." );

		return CreateFromMemory( mSource );
	}

	UINT ShaderGL::CreateFromMemory( char* source )
	{
		mSource = source;

		// Create a new program.
		//
		mProgram = glCreateProgram();

		if( mProgram < 0 )
			REPORT_ERROR( "Could not create shader program.", "Shader Loader Error" );

		if( strstr( mSource, "VERTEX_SHADER" ) != NULL )
		{
			const char *vshader[2] = { "#define VERSION_GL\n#define VERTEX_SHADER\n\0", mSource };
				
			if( !Compile( vshader, mVertexShader, GL_VERTEX_SHADER, 2 ))
				return S_FALSE;

			glAttachShader( mProgram, mVertexShader );
		}
			
		if( strstr( mSource, "GEOMETRY_SHADER" ) != NULL )
		{
			const char *gshader[2] = { "#define VERSION_GL\n#define GEOMETRY_SHADER\n\0", mSource };

			if( !Compile( gshader, mGeometryShader, GL_GEOMETRY_SHADER, 2 ))
				return S_FALSE;

			glAttachShader( mProgram, mGeometryShader );
		}

		if( strstr( mSource, "FRAGMENT_SHADER" ) != NULL )
		{
			const char *fshader[2] = { "#define VERSION_GL\n#define FRAGMENT_SHADER\n\0", mSource };

			if( !Compile( fshader, mFragmentShader, GL_FRAGMENT_SHADER, 2 ))
				return S_FALSE;

			glAttachShader( mProgram, mFragmentShader );
		}

		TRACE( "Shader Compiled Successfully!" );

		// Link the shaders.
		//
		int didCompile;

		glLinkProgram( mProgram );
		glGetProgramiv( mProgram, GL_LINK_STATUS, &didCompile );

		// Report errors.
		//
		if( didCompile == GL_FALSE )
		{
			char* compileLog;
			int length;

			glGetProgramiv( mProgram, GL_INFO_LOG_LENGTH, &length );

			compileLog = (char*)malloc( length );
 
			glGetProgramInfoLog( mProgram, length, &length, compileLog );

			TRACE( "\nLink Shader Log" );
			TRACE( compileLog );

			free( compileLog );

			Release();

			return S_FALSE;
		}

		// Create attributes.
		//
		CreateAttribute( "Position",	ATTRIB_POSITION );
		CreateAttribute( "TexCoord0",	ATTRIB_TEXCOORD0 );
		CreateAttribute( "TexCoord1",	ATTRIB_TEXCOORD1 );
		CreateAttribute( "TexCoord2",	ATTRIB_TEXCOORD2 );
		CreateAttribute( "QuadCoord0",	ATTRIB_QUADCOORD0 );
		CreateAttribute( "QuadCoord1",	ATTRIB_QUADCOORD1 );
		CreateAttribute( "QuadCoord2",	ATTRIB_QUADCOORD2 );
		CreateAttribute( "Normal",		ATTRIB_NORMAL );
		CreateAttribute( "Color",		ATTRIB_COLOR );
		CreateAttribute( "Tangent",		ATTRIB_TANGENT );
		CreateAttribute( "BoneCount",	ATTRIB_BONE_COUNT );
		CreateAttribute( "BoneMatrix",	ATTRIB_BONE_INDEX );
		CreateAttribute( "BoneWeight",	ATTRIB_BONE_WEIGHT );

		if( CreateAttribute( "Matrix",	ATTRIB_MATRIX ))
			mAttributeSize += 3;

		// Create layout.
		//
		VertexLayoutGL* layout = new VertexLayoutGL();

		UINT size = (UINT)mAttribute.size();
		for( UINT x = 0; x < size; ++x )
		{
			layout->AddAttribute( mAttribute[ x ] );
		}

		mLayout = std::shared_ptr< VertexLayout >(layout);

		// Create uniforms.
		//
		UINT numTexture = 0;

		CreateUniform( "_WVP",			UNIFORM_WVP );
		CreateUniform( "_World",		UNIFORM_WORLD );
		CreateUniform( "_InvWorld",		UNIFORM_INV_WORLD );
		CreateUniform( "_View",			UNIFORM_VIEW );
		CreateUniform( "_InvView",		UNIFORM_INV_VIEW );
		CreateUniform( "_Proj",			UNIFORM_PROJ );
		CreateUniform( "_InvProj",		UNIFORM_INV_PROJ );
		CreateUniform( "_Ambient",		UNIFORM_AMBIENT );
		CreateUniform( "_Diffuse",		UNIFORM_DIFFUSE );
		CreateUniform( "_Specular",		UNIFORM_SPECULAR );
		CreateUniform( "_SpecComp",		UNIFORM_SPEC_COMP );
		CreateUniform( "_LightPos",		UNIFORM_LIGHT_POS );
		CreateUniform( "_LightColor",	UNIFORM_LIGHT_COLOR );
		CreateUniform( "_LightAttn",	UNIFORM_LIGHT_ATTN );
		CreateUniform( "_CameraPos",	UNIFORM_CAMERA_POS );
		CreateUniform( "_Bones",		UNIFORM_BONES );
		CreateUniform( "_DeltaTime",	UNIFORM_DELTA_TIME );

		// Create texture samplers.
		//
		char texName[16];
		for( int x = 0; x < MAX_TEXTURES; ++x )
		{
			sprintf_s( texName, "_Texture%d", x );

			if( CreateUniform( texName, UNIFORM_TEXTURE ))
				++numTexture;
		}

		if( numTexture > 0 )
		{
			mNumSamplers = numTexture;

			mSampler = new Sampler*[ numTexture ];

			for( UINT i = 0; i < numTexture; ++i )
				mSampler[ i ] = new SamplerGL();
		}

		TRACE( "Shader Created Successfully!" );

		return S_OK;
	}

	///////////////////////////////////

	int ShaderGL::Compile( const GLchar** source, GLuint& shader, GLenum type, GLsizei count )
	{
		_ASSERT( type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER || type == GL_GEOMETRY_SHADER );

		shader = glCreateShader( type );

		glShaderSource( shader, count, source, NULL );
		glCompileShader( shader );

		int didCompile = 0;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &didCompile );

		// Report compile errors.
		//
		if( didCompile == GL_FALSE )
		{
			int length;
			char* compileLog;

			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
 
			compileLog = (char*)malloc( length );
 
			glGetShaderInfoLog( shader, length, &length, compileLog );

			TRACE( "\nShader Compile Log" );
			TRACE( compileLog );

			free( compileLog );
				
			glDeleteShader( shader );

			return 0;
		}

		return 1;
	}

	///////////////////////////////////

	void ShaderGL::SetFloat( UINT uniform, float data )
	{
		glUniform1f( mUniformGL[ uniform ], data );
	}

	void ShaderGL::SetFloat2( UINT uniform, float* data )
	{
		glUniform2fv( mUniformGL[ uniform ], 1, data );
	}

	void ShaderGL::SetFloat3( UINT uniform, float* data )
	{
		glUniform3fv( mUniformGL[ uniform ], 1, data );
	}

	void ShaderGL::SetFloat4( UINT uniform, float* data )
	{
		glUniform4fv( mUniformGL[ uniform ], 1, data );
	}

	void ShaderGL::SetMatrix( UINT uniform, float* matrix )
	{
		glUniformMatrix4fv( mUniformGL[ uniform ], 1, false, matrix );
	}

	void ShaderGL::SetTexture( UINT uniform, Texture* texture )
	{
		glUniform1i( mUniformGL[ uniform ], mTextureLevel );
			
		DWORD texID = GL_TEXTURE0 + mTextureLevel;
		glClientActiveTexture( texID );
		glActiveTexture( texID );

		glBindTexture( GL_TEXTURE_2D, ((TextureGL*)texture)->mID );

		static_cast< SamplerGL* >(mSampler[ mTextureLevel ])->Apply();
			
		++mTextureLevel;
	}

	void ShaderGL::SetSampler( UINT index, SamplerMode modeU, SamplerMode modeV, 
						SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter )
	{
		_ASSERT( index < mNumSamplers );

		static_cast< SamplerGL* >(mSampler[ index ])->Create( modeU, modeV, minFilter, magFilter, mipFilter );
	}

	///////////////////////////////////

	void ShaderGL::Enable()
	{
		glUseProgram( mProgram );

		mTextureLevel = 0;

		for( UINT i = 0; i < mAttributeSize; ++i )
			glEnableVertexAttribArray( i );
	}

	void ShaderGL::Disable()
	{
		for( UINT i = 0; i < mAttributeSize; ++i )
			glDisableVertexAttribArray( i );
	}
}
