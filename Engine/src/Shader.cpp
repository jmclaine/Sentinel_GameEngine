#include "Util.h"
#include "Shader.h"
#include "Texture.h"

namespace Sentinel
{
	Shader::Sampler::Sampler()
	{ }

	Shader::Sampler::~Sampler()
	{ }

	////////////////////////////////////

	Shader::Shader() :
		mSource(nullptr),
		mSampler(nullptr),
		mNumSamplers(0)
	{ }

	Shader::~Shader()
	{
		if (mSampler)
		{
			for (UINT x = 0; x < mNumSamplers; ++x)
				delete mSampler[x];

			delete[] mSampler;
		}

		free(mSource);
	}

	const char* Shader::Source()
	{
		return mSource;
	}

	const std::vector<VertexAttribute>& Shader::Attributes()
	{
		return mAttributes;
	}

	const std::vector<ShaderUniform>& Shader::Uniforms()
	{
		return mUniforms;
	}

	std::weak_ptr<VertexLayout> Shader::Layout()
	{
		return std::move(std::weak_ptr<VertexLayout>(mLayout));
	}
}