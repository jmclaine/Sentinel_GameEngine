#pragma once

#include "Common.h"

namespace Sentinel
{
	#define MAX_BONE_MATRICES		100
	#define MAX_TEXTURES			4
	#define MAX_LIGHTS				1

	class Texture;
	
	enum SamplerMode
	{
		MODE_WRAP,
		MODE_CLAMP,

		NUM_MODES
	};

	enum SamplerFilter
	{
		FILTER_POINT,
		FILTER_LINEAR,
		
		NUM_FILTERS
	};

	class SENTINEL_DLL Shader
	{
	public:

		class Sampler
		{
		protected:

			Sampler();

		public:

			virtual ~Sampler();
		};

	protected:

		char*				mShaderSource;
		
		std::string			mAttribute;
		std::string			mUniform;

		UINT				mVertexSize;

		Sampler**			mSampler;
		UINT				mNumSamplers;

		////////////////////////////////////

		Shader();

	public:

		virtual ~Shader();

		const char*			Source();
		const std::string&	Attribute();
		const std::string&	Uniform();

		UINT  				VertexSize();

		virtual void		Release() = 0;

		virtual void		ApplyPass() = 0;
		virtual void		ApplyLayout() = 0;

		virtual void		SetFloat( UINT uniform, float data ) = 0;
		virtual void		SetFloat2( UINT uniform, float* data ) = 0;
		virtual void		SetFloat3( UINT uniform, float* data ) = 0;
		virtual void		SetFloat4( UINT uniform, float* data ) = 0;
		virtual void		SetMatrix( UINT uniform, float* data ) = 0;
		virtual void		SetTexture( UINT uniform, Texture* texture ) = 0;

		virtual void		SetSampler( UINT index, SamplerMode modeU, SamplerMode modeV, 
										SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter = NUM_FILTERS ) = 0;

	protected:

		virtual void		CreateUniform( const char* name ) = 0;

		void  				ProcessUniforms();
	};
}
