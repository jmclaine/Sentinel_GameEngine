#pragma once

#include "CommonGL.h"
#include "Shader.h"

namespace Sentinel
{
	class Texture;

	class ShaderGL : public Shader
	{
	private:
		// stores states to prevent setting values internal to OpenGL unncessarily
		static GLenum CURR_ACTIVE;
		static Texture* CURR_TEXTURE[32];

	public:
		class SamplerGL : public Sampler
		{
		private:
			static SamplerGL CURR_STATE;

		public:
			static GLint SAMPLER_MODE[(BYTE)SamplerMode::COUNT];

		protected:
			GLint mWrapS;
			GLint mWrapT;

			GLint mMinFilter;
			GLint mMagFilter;

		public:
			SamplerGL(
				GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT,
				GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR);

			void Create(
				SamplerMode modeU, SamplerMode modeV,
				SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter);

			void Apply();
		};

		class SamplerCubeGL : public SamplerGL
		{
		private:
			static SamplerCubeGL CURR_STATE;

		protected:
			GLint mWrapR;

		public:
			SamplerCubeGL(
				GLint wrapS = GL_CLAMP_TO_EDGE, GLint wrapT = GL_CLAMP_TO_EDGE, GLint wrapR = GL_CLAMP_TO_EDGE,
				GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR);

			void Create(
				SamplerMode modeU, SamplerMode modeV, SamplerMode modeW,
				SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter);

			void Apply();
		};

	private:
		GLuint mProgram;

		GLuint mVertexShader;
		GLuint mGeometryShader;
		GLuint mFragmentShader;

		UINT mTextureLevel;

		UINT mAttributeSize;

	public:
		GLint mAttributeGL[(BYTE)VertexAttribute::COUNT];

		std::vector<GLint> mUniformsGL;

		///////////////////////////////////

		ShaderGL();
		~ShaderGL();

		void Release();

		GLuint Program();
		UINT AttributeSize();

	private:
		bool CreateAttribute(char* name, VertexAttribute type);
		bool CreateUniform(char* name, ShaderUniform type);

	public:
		UINT CreateFromFile(std::string filename);
		UINT CreateFromMemory(char* source);

	private:
		bool Compile(const GLchar** source, GLuint& shader, GLenum type, GLsizei count);

	public:
		void SetFloat(UINT uniform, float* data, UINT count = 1);
		void SetFloat2(UINT uniform, float* data, UINT count = 1);
		void SetFloat3(UINT uniform, float* data, UINT count = 1);
		void SetFloat4(UINT uniform, float* data, UINT count = 1);
		void SetMatrix(UINT uniform, float* matrix, UINT count = 1);
		void SetTexture(UINT uniform, Texture* texture);
		void SetTextureCube(UINT uniform, Texture* texture);

		void SetSampler(
			UINT index,
			SamplerMode modeU, SamplerMode modeV,
			SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter);

		void SetSamplerCube(
			UINT index,
			SamplerMode modeU, SamplerMode modeV, SamplerMode modeW,
			SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter);

		///////////////////////////////////

		void Enable();
		void Disable();
	};
}
