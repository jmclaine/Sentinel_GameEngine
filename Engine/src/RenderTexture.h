#pragma once

namespace Sentinel
{
	class Texture;

	class RenderTexture
	{
	protected:
		Texture* mTexture;

		//////////////////

		RenderTexture(Texture* texture);

	public:
		virtual ~RenderTexture();

		Texture* GetTexture() const;
	};
}
