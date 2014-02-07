#pragma once

#include <vector>

#include "VertexLayout.h"

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

	enum UniformType
	{
		UNIFORM_WVP,
		UNIFORM_WORLD,
		UNIFORM_INV_WORLD,
		UNIFORM_VIEW,
		UNIFORM_INV_VIEW,
		UNIFORM_PROJ,
		UNIFORM_INV_PROJ,
		UNIFORM_TEXTURE,
		UNIFORM_AMBIENT,
		UNIFORM_DIFFUSE,
		UNIFORM_SPECULAR,
		UNIFORM_SPEC_COMP,
		UNIFORM_LIGHT_POS,
		UNIFORM_LIGHT_DIR,
		UNIFORM_LIGHT_COLOR,
		UNIFORM_LIGHT_ATTN,
		UNIFORM_LIGHT_MATRIX,
		UNIFORM_LIGHT_CUBE_MATRIX,
		UNIFORM_LIGHT_TEXTURE_CUBE,
		UNIFORM_CAMERA_POS,
		UNIFORM_BONES,
		UNIFORM_DELTA_TIME,
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

		char*			mSource;
		
		std::vector< AttributeType > mAttribute;
		std::vector< UniformType >   mUniform;

		std::shared_ptr< VertexLayout > mLayout;

		Sampler**		mSampler;
		UINT			mNumSamplers;

		////////////////////////////////////

		Shader();

	public:

		virtual ~Shader();

		const char*		Source();
		const std::vector< AttributeType >&   Attribute();
		const std::vector< UniformType >&     Uniform();
		const std::shared_ptr< VertexLayout > Layout();

		virtual void	Release() = 0;

		////////////////////////////////////
		
		virtual void	SetFloat( UINT uniform, float* data, UINT count = 1 ) = 0;
		virtual void	SetFloat2( UINT uniform, float* data, UINT count = 1 ) = 0;
		virtual void	SetFloat3( UINT uniform, float* data, UINT count = 1 ) = 0;
		virtual void	SetFloat4( UINT uniform, float* data, UINT count = 1 ) = 0;
		virtual void	SetMatrix( UINT uniform, float* data, UINT count = 1 ) = 0;
		virtual void	SetTexture( UINT uniform, Texture* texture ) = 0;
		virtual void	SetTextureCube( UINT uniform, Texture* texture ) = 0;

		virtual void	SetSampler( UINT index, SamplerMode modeU, SamplerMode modeV, 
									SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter = NUM_FILTERS ) = 0;

		////////////////////////////////////

		virtual void	Enable() = 0;
		virtual void	Disable() = 0;
	};
}
