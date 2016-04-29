#include "RenderTexture.h"
#include "Texture.h"

namespace Sentinel
{
	RenderTexture::RenderTexture(Texture* texture) :
		mTexture(texture)
	{ }

	RenderTexture::~RenderTexture()
	{ }

	Texture* RenderTexture::GetTexture() const
	{
		return mTexture;
	}
}
