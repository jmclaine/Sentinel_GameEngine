#pragma once

#include <vector>

#if (_MSC_VER > 1600)
#include <memory>
#endif

#include "VertexLayout.h"
#include "Types.h"

namespace Sentinel
{
#define MAX_BONE_MATRICES	100
#define MAX_TEXTURES		4
#define MAX_LIGHTS			1

	class Texture;

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

		std::vector<VertexAttribute::Type> mAttributes;
		std::vector<ShaderUniform::Type> mUniforms;

		std::shared_ptr<VertexLayout> mLayout;

		Sampler** mSampler;
		UINT mNumSamplers;

		////////////////////////////////////

		Shader();

	public:

		virtual ~Shader();

		const char* Source();
		const std::vector<VertexAttribute::Type>& Attributes();
		const std::vector<ShaderUniform::Type>& Uniforms();
		const std::shared_ptr<VertexLayout> Layout();

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
			SamplerMode::Type modeU, SamplerMode::Type modeV,
			SamplerFilter::Type minFilter, SamplerFilter::Type magFilter, SamplerFilter::Type mipFilter = SamplerFilter::UNKNOWN) = 0;

		virtual void SetSamplerCube(
			UINT index,
			SamplerMode::Type modeU, SamplerMode::Type modeV, SamplerMode::Type modeW,
			SamplerFilter::Type minFilter, SamplerFilter::Type magFilter, SamplerFilter::Type mipFilter = SamplerFilter::UNKNOWN) = 0;

		////////////////////////////////////

		virtual void Enable() = 0;
		virtual void Disable() = 0;
	};
}
