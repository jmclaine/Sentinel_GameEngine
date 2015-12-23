#include "ShaderGL.h"
#include "TextureGL.h"
#include "VertexLayoutGL.h"
#include "Archive.h"
#include "Debug.h"
#include "Memory.h"

namespace Sentinel
{
#define S_OK 0
#define S_FALSE 1

#define APPLY_SAMPLER(target, pname, param)\
	glTexParameteri(target, pname, param);

	/*
	#define APPLY_SAMPLER(target, pname, param)\
	if (CURR_STATE.param != param)\
	{\
	glTexParameteri(target, pname, param);\
	CURR_STATE.param = param;\
	}
	*/

	GLenum ShaderGL::CURR_ACTIVE = -1;
	Texture* ShaderGL::CURR_TEXTURE[32] = {};

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
			mAttributes.push_back(type);

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
			mUniforms.push_back(type);
			mUniformsGL.push_back(uniform);

			return true;
		}

		return false;
	}

	///////////////////////////////////

	UINT ShaderGL::CreateFromFile(std::string filename)
	{
		if (Archive::ToBuffer(filename.c_str(), mSource) == 0)
		{
			Debug::ShowError(
				STREAM("Could not open '" << filename << "'"),
				STREAM("Shader Loader Error"));

			return S_FALSE;
		}

		Debug::Log(STREAM("Compiling '" << filename << "'..."));

		return CreateFromMemory(mSource);
	}

	UINT ShaderGL::CreateFromMemory(char* source)
	{
		mSource = source;

		mProgram = glCreateProgram();

		if (mProgram < 0)
		{
			Debug::ShowError(
				"Could not create shader program.",
				"Shader Loader Error");
		}

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

		Debug::Log("Shader Compiled Successfully!");

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

			Debug::Log("\nLink Shader Log");
			Debug::Log(compileLog);

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

		UINT size = (UINT)mAttributes.size();
		for (UINT x = 0; x < size; ++x)
		{
			layout->AddAttribute(mAttributes[x]);
		}

		mLayout = std::shared_ptr< VertexLayout >(layout);

		// Create uniforms.
		//
		CreateUniform("_WorldViewProj", ShaderUniform::WORLD_VIEW_PROJ);
		CreateUniform("_WorldView", ShaderUniform::WORLD_VIEW);
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

		Debug::Log("Shader Created Successfully!");

		return S_OK;
	}

	///////////////////////////////////

	bool ShaderGL::Compile(const GLchar** source, GLuint& shader, GLenum type, GLsizei count)
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
				Debug::Log("\nVertex Shader Compile Log");
				break;

			case GL_GEOMETRY_SHADER:
				Debug::Log("\nGeometry Shader Compile Log");
				break;

			case GL_FRAGMENT_SHADER:
				Debug::Log("\nFragment Shader Compile Log");
				break;
			}

			Debug::Log(compileLog);

			free(compileLog);

			glDeleteShader(shader);

			return false;
		}

		return true;
	}

	///////////////////////////////////

	void ShaderGL::SetFloat(UINT uniform, float* data, UINT count)
	{
		glUniform1fv(mUniformsGL[uniform], count, data);
	}

	void ShaderGL::SetFloat2(UINT uniform, float* data, UINT count)
	{
		glUniform2fv(mUniformsGL[uniform], count, data);
	}

	void ShaderGL::SetFloat3(UINT uniform, float* data, UINT count)
	{
		glUniform3fv(mUniformsGL[uniform], count, data);
	}

	void ShaderGL::SetFloat4(UINT uniform, float* data, UINT count)
	{
		glUniform4fv(mUniformsGL[uniform], count, data);
	}

	void ShaderGL::SetMatrix(UINT uniform, float* matrix, UINT count)
	{
		glUniformMatrix4fv(mUniformsGL[uniform], count, false, matrix);
	}

	void ShaderGL::SetTexture(UINT uniform, Texture* texture)
	{
		_ASSERT(texture != NULL);

		glUniform1i(mUniformsGL[uniform], mTextureLevel);

		Texture*& currTex = CURR_TEXTURE[mTextureLevel];

		if (currTex != texture)
		{
			GLenum texID = GL_TEXTURE0 + mTextureLevel;

			if (CURR_ACTIVE != texID)
			{
				glActiveTexture(texID);

				CURR_ACTIVE = texID;
			}

			TextureGL* tex = static_cast<TextureGL*>(texture);

			glBindTexture(GL_TEXTURE_2D, tex->ID());

			currTex = texture;
		}

		static_cast<SamplerGL*>(mSampler[mTextureLevel++])->Apply();
	}

	void ShaderGL::SetTextureCube(UINT uniform, Texture* texture)
	{
		glUniform1i(mUniformsGL[uniform], mTextureLevel);

		static Texture* CURR_CUBE = NULL;

		if (CURR_CUBE != texture)
		{
			if (CURR_ACTIVE != GL_TEXTURE_CUBE_MAP)
			{
				glActiveTexture(GL_TEXTURE_CUBE_MAP);

				CURR_ACTIVE = GL_TEXTURE_CUBE_MAP;
			}

			TextureGL* tex = static_cast<TextureGL*>(texture);

			glBindTexture(GL_TEXTURE_CUBE_MAP, tex->ID());

			CURR_CUBE = texture;
		}

		static_cast<SamplerCubeGL*>(mSampler[mTextureLevel++])->Apply();
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
