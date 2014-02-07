#pragma once

#include "CommonGL.h"
#include "Shader.h"

namespace Sentinel
{
	class Texture;

	class ShaderGL : public Shader
	{
	public:

		class SamplerGL : public Sampler
		{
		private:

			GLint	mWrapS;
			GLint	mWrapT;

			GLint	mMinFilter;
			GLint	mMagFilter;

		public:

			SamplerGL();

			void Create( SamplerMode modeU, SamplerMode modeV, SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter );

			void Apply();
		};

	private:
		
		GLuint		mProgram;

		GLuint		mVertexShader;
		GLuint		mGeometryShader;
		GLuint		mFragmentShader;

		UINT		mTextureLevel;

		UINT		mAttributeSize;

	public:

		GLint		mAttributeGL[ NUM_ATTRIBUTES ];

		std::vector< GLint > mUniformGL;

		///////////////////////////////////

		ShaderGL();
		~ShaderGL();

		void		Release();

		GLuint		Program();
		UINT		AttributeSize();

	private:

		bool		CreateAttribute( char* name, AttributeType type );
		bool		CreateUniform( char* name, UniformType type );

	public:

		UINT		CreateFromFile( std::string filename );
		UINT		CreateFromMemory( char* source );

	private:

		int			Compile( const GLchar** source, GLuint& shader, GLenum type, GLsizei count );
		
	public:

		void		SetFloat( UINT uniform, float* data, UINT count = 1 );
		void		SetFloat2( UINT uniform, float* data, UINT count = 1 );
		void		SetFloat3( UINT uniform, float* data, UINT count = 1 );
		void		SetFloat4( UINT uniform, float* data, UINT count = 1 );
		void		SetMatrix( UINT uniform, float* matrix, UINT count = 1 );
		void		SetTexture( UINT uniform, Texture* texture );
		void		SetTextureCube( UINT uniform, Texture* texture );

		void		SetSampler( UINT index, SamplerMode modeU, SamplerMode modeV, 
								SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter );

		///////////////////////////////////

		void		Enable();
		void		Disable();
	};
}
