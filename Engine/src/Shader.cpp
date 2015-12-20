#include "Util.h"
#include "Shader.h"
#include "Texture.h"

namespace Sentinel
{
	Shader::Sampler::Sampler()
	{}

	Shader::Sampler::~Sampler()
	{}

	////////////////////////////////////

	Shader::Shader() :
		mSource(NULL),
		mSampler(NULL),
		mNumSamplers(0)
	{}

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

	const std::vector<VertexAttribute::Type>& Shader::Attributes()
	{
		return mAttributes;
	}

	const std::vector<ShaderUniform::Type>& Shader::Uniforms()
	{
		return mUniforms;
	}

	const std::shared_ptr<VertexLayout> Shader::Layout()
	{
		return mLayout;
	}
}