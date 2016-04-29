#pragma once

#include <vector>

#if (_MSC_VER > 1600)
#include <memory>
#endif

#include "VertexLayout.h"

namespace Sentinel
{
#define MAX_BONE_MATRICES	100
#define MAX_TEXTURES		4
#define MAX_LIGHTS			1

	class Texture;

	enum class ShaderUniform : BYTE
	{
		WORLD_VIEW_PROJ,
		WORLD_VIEW,
		WORLD,
		INV_WORLD,
		VIEW,
		INV_VIEW,
		PROJ,
		INV_PROJ,
		TEXTURE,
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		SPEC_COMP,
		LIGHT_POS,
		LIGHT_DIR,
		LIGHT_COLOR,
		LIGHT_ATTN,
		LIGHT_TEXTURE_CUBE,
		LIGHT_MATRIX,
		LIGHT_CUBE_MATRIX,
		SHADOW_BLEND,
		SHADOW_QUALITY,
		CAMERA_POS,
		BONES,
		DELTA_TIME,
	};

	enum class SamplerMode
	{
		WRAP,
		CLAMP,
		CLAMP_TO_EDGE,

		COUNT,
		UNKNOWN
	};

	enum class SamplerFilter
	{
		POINT,
		LINEAR,

		UNKNOWN
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
		char* mSource;

		std::vector<VertexAttribute> mAttributes;
		std::vector<ShaderUniform> mUniforms;

		std::shared_ptr<VertexLayout> mLayout;

		Sampler** mSampler;
		UINT mNumSamplers;

		////////////////////////////////////

		Shader();

	public:
		virtual ~Shader();

		const char* Source();
		const std::vector<VertexAttribute>& Attributes();
		const std::vector<ShaderUniform>& Uniforms();
		std::weak_ptr<VertexLayout> Layout();

		virtual void Release() = 0;

		////////////////////////////////////

		virtual void SetFloat(UINT uniform, float* data, UINT count = 1) = 0;
		virtual void SetFloat2(UINT uniform, float* data, UINT count = 1) = 0;
		virtual void SetFloat3(UINT uniform, float* data, UINT count = 1) = 0;
		virtual void SetFloat4(UINT uniform, float* data, UINT count = 1) = 0;
		virtual void SetMatrix(UINT uniform, float* data, UINT count = 1) = 0;
		virtual void SetTexture(UINT uniform, Texture* texture) = 0;
		virtual void SetTextureCube(UINT uniform, Texture* texture) = 0;

		virtual void SetSampler(
			UINT index,
			SamplerMode modeU, SamplerMode modeV,
			SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter = SamplerFilter::UNKNOWN) = 0;

		virtual void SetSamplerCube(
			UINT index,
			SamplerMode modeU, SamplerMode modeV, SamplerMode modeW,
			SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter = SamplerFilter::UNKNOWN) = 0;

		////////////////////////////////////

		virtual void Enable() = 0;
		virtual void Disable() = 0;
	};
}
