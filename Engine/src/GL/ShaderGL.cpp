#include "ShaderGL.h"
#include "TextureGL.h"
#include "VertexLayoutGL.h"
#include "Archive.h"

namespace Sentinel
{
	/*
	#define APPLY_SAMPLER( target, pname, param )\
	if( CURR_STATE.param != param )\
	{\
	glTexParameteri( target, pname, param );\
	CURR_STATE.param = param;\
	}
	*/
#define APPLY_SAMPLER(target, pname, param)\
	glTexParameteri(target, pname, param);

	ShaderGL::SamplerGL::SamplerGL(
		GLint wrapS, GLint wrapT,
		GLint minFilter, GLint magFilter) :
		mWrapS(wrapS),
		mWrapT(wrapT),
		mMinFilter(minFilter),
		mMagFilter(magFilter)
	{}

	void ShaderGL::SamplerGL::Create(
		SamplerMode::Type modeU, SamplerMode::Type modeV,
		SamplerFilter::Type minFilter, SamplerFilter::Type magFilter, SamplerFilter::Type mipFilter)
	{
		mWrapS = SAMPLER_MODE[modeU];
		mWrapT = SAMPLER_MODE[modeV];

		if (minFilter == SamplerFilter::LINEAR)
		{
			if (mipFilter == SamplerFilter::LINEAR)
			{
				mMinFilter = GL_LINEAR_MIPMAP_LINEAR;
			}
			else if (mipFilter == SamplerFilter::POINT)
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
			if (mipFilter == SamplerFilter::LINEAR)
			{
				mMinFilter = GL_NEAREST_MIPMAP_LINEAR;
			}
			else if (mipFilter == SamplerFilter::POINT)
			{
				mMinFilter = GL_NEAREST_MIPMAP_NEAREST;
			}
			else
			{
				mMinFilter = GL_NEAREST;
			}
		}

		if (magFilter == SamplerFilter::LINEAR)
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
		APPLY_SAMPLER(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrapS);
		APPLY_SAMPLER(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrapT);

		APPLY_SAMPLER(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mMinFilter);
		APPLY_SAMPLER(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mMagFilter);
	}

	ShaderGL::SamplerGL ShaderGL::SamplerGL::CURR_STATE(SamplerMode::UNKNOWN, SamplerMode::UNKNOWN,
		SamplerFilter::UNKNOWN, SamplerFilter::UNKNOWN);

	GLint ShaderGL::SamplerGL::SAMPLER_MODE[] =
	{
		GL_REPEAT,
		GL_CLAMP,
		GL_CLAMP_TO_EDGE
	};

	//////////////////////////////////////////////////////////////////////

	ShaderGL::SamplerCubeGL::SamplerCubeGL(
		GLint wrapS, GLint wrapT, GLint wrapR,
		GLint minFilter, GLint magFilter) :
		mWrapR(wrapR)
	{
		mWrapS = wrapS;
		mWrapT = wrapT;

		mMinFilter = minFilter;
		mMagFilter = magFilter;
	}

	void ShaderGL::SamplerCubeGL::Create(
		SamplerMode::Type modeU, SamplerMode::Type modeV, SamplerMode::Type modeW,
		SamplerFilter::Type minFilter, SamplerFilter::Type magFilter, SamplerFilter::Type mipFilter)
	{
		SamplerGL::Create(modeU, modeV, minFilter, magFilter, mipFilter);

		mWrapR = SamplerGL::SAMPLER_MODE[modeW];
	}

	void ShaderGL::SamplerCubeGL::Apply()
	{
		APPLY_SAMPLER(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, mWrapS);
		APPLY_SAMPLER(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, mWrapT);
		APPLY_SAMPLER(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, mWrapR);

		APPLY_SAMPLER(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mMinFilter);
		APPLY_SAMPLER(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mMagFilter);
	}

	ShaderGL::SamplerCubeGL ShaderGL::SamplerCubeGL::CURR_STATE(
		SamplerMode::UNKNOWN, SamplerMode::UNKNOWN, SamplerMode::UNKNOWN,
		SamplerFilter::UNKNOWN, SamplerFilter::UNKNOWN);

	//////////////////////////////////////////////////////////////////////

	ShaderGL::ShaderGL() :
		mProgram(0),
		mVertexShader(0),
		mGeometryShader(0),
		mFragmentShader(0),
		mTextureLevel(0),
		mAttributeSize(0)
	{}

	ShaderGL::~ShaderGL()
	{
		Release();
	}

	void ShaderGL::Release()
	{
		if (mSampler)
		{
			for (UINT x = 0; x < mNumSamplers; ++x)
			{
				SAFE_DELETE(mSampler[x]);
			}

			SAFE_DELETE_ARRAY(mSampler);
		}

		if (mProgram != 0)
		{
			glDeleteProgram(mProgram);
			mProgram = 0;

			glDeleteShader(mVertexShader);
			mVertexShader = 0;

			glDeleteShader(mGeometryShader);
			mGeometryShader = 0;

			glDeleteShader(mFragmentShader);
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

	bool ShaderGL::CreateAttribute(char* name, VertexAttribute::Type type)
	{
		GLint attrib = glGetAttribLocation(mProgram, name);

		mAttributeGL[type] = attrib;

		if (attrib != -1)
		{
			mAttribute.push_back(type);

			++mAttributeSize;

			return true;
		}

		return false;
	}

	bool ShaderGL::CreateUniform(char* name, ShaderUniform::Type type)
	{
		GLint uniform = glGetUniformLocation(mProgram, name);

		if (uniform != -1)
		{
			mUniform.push_back(type);
			mUniformGL.push_back(uniform);

			return true;
		}

		return false;
	}

	///////////////////////////////////

	UINT ShaderGL::CreateFromFile(std::string filename)
	{
		if (Archive::ToBuffer(filename.c_str(), mSource) == 0)
		{
			REPORT_ERROR("Could not open '" << filename << "'", "Shader Loader Error");

			return S_FALSE;
		}

		TRACE("Compiling '" << filename << "'...");

		return CreateFromMemory(mSource);
	}

	UINT ShaderGL::CreateFromMemory(char* source)
	{
		mSource = source;

		mProgram = glCreateProgram();

		if (mProgram < 0)
			REPORT_ERROR("Could not create shader program.", "Shader Loader Error");

		// Compile Vertex Shader.
		//
		if (strstr(mSource, "VERTEX_SHADER") != NULL)
		{
			const char *vshader[2] = { "#version 330\n#define VERSION_GL\n#define VERTEX_SHADER\n\0", mSource };

			if (!Compile(vshader, mVertexShader, GL_VERTEX_SHADER, 2))
				return S_FALSE;

			glAttachShader(mProgram, mVertexShader);
		}

		// Compile Geometry Shader.
		//
		if (strstr(mSource, "GEOMETRY_SHADER") != NULL)
		{
			const char *gshader[2] = { "#version 330\n#define VERSION_GL\n#define GEOMETRY_SHADER\n\0", mSource };

			if (!Compile(gshader, mGeometryShader, GL_GEOMETRY_SHADER, 2))
				return S_FALSE;

			glAttachShader(mProgram, mGeometryShader);
		}

		// Compile Fragment Shader.
		//
		if (strstr(mSource, "FRAGMENT_SHADER") != NULL)
		{
			const char *fshader[2] = { "#version 330\n#define VERSION_GL\n#define FRAGMENT_SHADER\n\0", mSource };

			if (!Compile(fshader, mFragmentShader, GL_FRAGMENT_SHADER, 2))
				return S_FALSE;

			glAttachShader(mProgram, mFragmentShader);
		}

		TRACE("Shader Compiled Successfully!");

		// Link the shaders.
		//
		int didCompile;

		glLinkProgram(mProgram);
		glGetProgramiv(mProgram, GL_LINK_STATUS, &didCompile);

		// Report errors.
		//
		if (didCompile == GL_FALSE)
		{
			char* compileLog;
			int length;

			glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &length);

			compileLog = (char*)malloc(length);

			glGetProgramInfoLog(mProgram, length, &length, compileLog);

			TRACE("\nLink Shader Log");
			TRACE(compileLog);

			free(compileLog);

			Release();

			return S_FALSE;
		}

		// Create attributes.
		//
		CreateAttribute("Position", VertexAttribute::POSITION);
		CreateAttribute("TexCoord0", VertexAttribute::TEXCOORD0);
		CreateAttribute("TexCoord1", VertexAttribute::TEXCOORD1);
		CreateAttribute("TexCoord2", VertexAttribute::TEXCOORD2);
		CreateAttribute("QuadCoord0", VertexAttribute::QUADCOORD0);
		CreateAttribute("QuadCoord1", VertexAttribute::QUADCOORD1);
		CreateAttribute("QuadCoord2", VertexAttribute::QUADCOORD2);
		CreateAttribute("Normal", VertexAttribute::NORMAL);
		CreateAttribute("Color", VertexAttribute::COLOR);
		CreateAttribute("Tangent", VertexAttribute::TANGENT);
		CreateAttribute("BoneCount", VertexAttribute::BONE_COUNT);
		CreateAttribute("BoneMatrix", VertexAttribute::BONE_INDEX);
		CreateAttribute("BoneWeight", VertexAttribute::BONE_WEIGHT);

		if (CreateAttribute("Matrix", VertexAttribute::MATRIX))
			mAttributeSize += 3;

		// Create layout.
		//
		VertexLayoutGL* layout = new VertexLayoutGL();

		UINT size = (UINT)mAttribute.size();
		for (UINT x = 0; x < size; ++x)
		{
			layout->AddAttribute(mAttribute[x]);
		}

		mLayout = std::shared_ptr< VertexLayout >(layout);

		// Create uniforms.
		//
		CreateUniform("_WVP", ShaderUniform::WVP);
		CreateUniform("_World", ShaderUniform::WORLD);
		CreateUniform("_InvWorld", ShaderUniform::INV_WORLD);
		CreateUniform("_View", ShaderUniform::VIEW);
		CreateUniform("_InvView", ShaderUniform::INV_VIEW);
		CreateUniform("_Proj", ShaderUniform::PROJ);
		CreateUniform("_InvProj", ShaderUniform::INV_PROJ);
		CreateUniform("_Ambient", ShaderUniform::AMBIENT);
		CreateUniform("_Diffuse", ShaderUniform::DIFFUSE);
		CreateUniform("_Specular", ShaderUniform::SPECULAR);
		CreateUniform("_SpecComp", ShaderUniform::SPEC_COMP);
		CreateUniform("_LightPos", ShaderUniform::LIGHT_POS);
		CreateUniform("_LightDir", ShaderUniform::LIGHT_DIR);
		CreateUniform("_LightColor", ShaderUniform::LIGHT_COLOR);
		CreateUniform("_LightAttn", ShaderUniform::LIGHT_ATTN);
		CreateUniform("_LightMatrix", ShaderUniform::LIGHT_MATRIX);
		CreateUniform("_LightCubeMatrix", ShaderUniform::LIGHT_CUBE_MATRIX);
		CreateUniform("_CameraPos", ShaderUniform::CAMERA_POS);
		CreateUniform("_Bones", ShaderUniform::BONES);
		CreateUniform("_DeltaTime", ShaderUniform::DELTA_TIME);

		// Create texture samplers.
		//
		UINT numTextureCube = 0;
		if (CreateUniform("_TextureCube", ShaderUniform::LIGHT_TEXTURE_CUBE))
			++numTextureCube;

		UINT numTexture = 0;
		char texName[16];
		for (int x = 0; x < MAX_TEXTURES; ++x)
		{
			sprintf_s(texName, "_Texture%d", x);

			if (CreateUniform(texName, ShaderUniform::TEXTURE))
				++numTexture;
		}

		mNumSamplers = numTexture + numTextureCube;

		if (mNumSamplers > 0)
		{
			mSampler = new Sampler*[mNumSamplers];

			UINT index = 0;

			for (; index < numTextureCube; ++index)
				mSampler[index] = new SamplerCubeGL();

			for (; index < mNumSamplers; ++index)
				mSampler[index] = new SamplerGL();
		}

		TRACE("Shader Created Successfully!");

		return S_OK;
	}

	///////////////////////////////////

	int ShaderGL::Compile(const GLchar** source, GLuint& shader, GLenum type, GLsizei count)
	{
		_ASSERT(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER || type == GL_GEOMETRY_SHADER);

		shader = glCreateShader(type);

		glShaderSource(shader, count, source, NULL);
		glCompileShader(shader);

		int didCompile = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &didCompile);

		// Report compile errors.
		//
		if (didCompile == GL_FALSE)
		{
			int length;
			char* compileLog;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			compileLog = (char*)malloc(length);

			glGetShaderInfoLog(shader, length, &length, compileLog);

			switch (type)
			{
			case GL_VERTEX_SHADER:
				TRACE("\nVertex Shader Compile Log");
				break;

			case GL_GEOMETRY_SHADER:
				TRACE("\nGeometry Shader Compile Log");
				break;

			case GL_FRAGMENT_SHADER:
				TRACE("\nFragment Shader Compile Log");
				break;
			}

			TRACE(compileLog);

			free(compileLog);

			glDeleteShader(shader);

			return 0;
		}

		return 1;
	}

	///////////////////////////////////

	void ShaderGL::SetFloat(UINT uniform, float* data, UINT count)
	{
		glUniform1fv(mUniformGL[uniform], count, data);
	}

	void ShaderGL::SetFloat2(UINT uniform, float* data, UINT count)
	{
		glUniform2fv(mUniformGL[uniform], count, data);
	}

	void ShaderGL::SetFloat3(UINT uniform, float* data, UINT count)
	{
		glUniform3fv(mUniformGL[uniform], count, data);
	}

	void ShaderGL::SetFloat4(UINT uniform, float* data, UINT count)
	{
		glUniform4fv(mUniformGL[uniform], count, data);
	}

	void ShaderGL::SetMatrix(UINT uniform, float* matrix, UINT count)
	{
		glUniformMatrix4fv(mUniformGL[uniform], count, false, matrix);
	}

	void ShaderGL::SetTexture(UINT uniform, Texture* texture)
	{
		glUniform1i(mUniformGL[uniform], mTextureLevel);

		GLenum texID = GL_TEXTURE0 + mTextureLevel;

		static GLenum CURR_ACTIVE = 0;

		if (CURR_ACTIVE != texID)
		{
			glActiveTexture(texID);

			CURR_ACTIVE = texID;
		}

		static GLuint CURR_TEXTURE = -1;

		GLuint id = ((TextureGL*)texture)->ID();
		if (CURR_TEXTURE != id)
		{
			glBindTexture(GL_TEXTURE_2D, id);

			CURR_TEXTURE = id;
		}

		static_cast<SamplerGL*>(mSampler[mTextureLevel])->Apply();

		++mTextureLevel;
	}

	void ShaderGL::SetTextureCube(UINT uniform, Texture* texture)
	{
		glUniform1i(mUniformGL[uniform], mTextureLevel);

		static bool IS_ACTIVE = false;

		if (!IS_ACTIVE)
		{
			glClientActiveTexture(GL_TEXTURE_CUBE_MAP);

			IS_ACTIVE = true;
		}

		static GLuint CURR_TEXTURE = -1;

		GLuint id = ((TextureGL*)texture)->ID();
		if (CURR_TEXTURE != id)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, ((TextureGL*)texture)->ID());

			CURR_TEXTURE = id;
		}

		static_cast<SamplerCubeGL*>(mSampler[mTextureLevel])->Apply();

		++mTextureLevel;
	}

	void ShaderGL::SetSampler(
		UINT index, SamplerMode::Type modeU, SamplerMode::Type modeV,
		SamplerFilter::Type minFilter, SamplerFilter::Type magFilter, SamplerFilter::Type mipFilter)
	{
		_ASSERT(index < mNumSamplers);

		static_cast<SamplerGL*>(mSampler[index])->Create(modeU, modeV, minFilter, magFilter, mipFilter);
	}

	void ShaderGL::SetSamplerCube(
		UINT index, SamplerMode::Type modeU, SamplerMode::Type modeV, SamplerMode::Type modeW,
		SamplerFilter::Type minFilter, SamplerFilter::Type magFilter, SamplerFilter::Type mipFilter)
	{
		_ASSERT(index < mNumSamplers);

		static_cast<SamplerCubeGL*>(mSampler[index])->Create(modeU, modeV, modeW, minFilter, magFilter, mipFilter);
	}

	///////////////////////////////////

	void ShaderGL::Enable()
	{
		glUseProgram(mProgram);

		mTextureLevel = 0;

		for (UINT i = 0; i < mAttributeSize; ++i)
			glEnableVertexAttribArray(i);
	}

	void ShaderGL::Disable()
	{
		for (UINT i = 0; i < mAttributeSize; ++i)
			glDisableVertexAttribArray(i);
	}
}
