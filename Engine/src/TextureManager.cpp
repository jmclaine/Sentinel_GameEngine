#include "TextureManager.h"
#include "Archive.h"
#include "Renderer.h"
#include "Texture.h"
#include "zlib.h"

namespace Sentinel
{
	TextureManager::TextureManager(Renderer* renderer)
	{ }

	void TextureManager::Save(Archive& archive, Renderer* renderer)
	{
		UINT count = mData.size();
		archive.Write(&count);

		for (auto it : mData)
		{
			std::shared_ptr<Texture> texture = it.second;

			archive.Write(&it.first);

			short width = texture->Width();
			archive.Write(&width);

			short height = texture->Height();
			archive.Write(&height);

			BYTE* pixels = (BYTE*)renderer->GetTexturePixels(texture.get());

			if (!pixels)
				throw std::exception("Failed to GetTexturePixels.");

			UINT  size = (width * 4) * height; // RGBA; 32-bit color
			ULONG bound = compressBound(size);

			BYTE* comp_pixels = new BYTE[bound];

			compress(comp_pixels, &bound, pixels, size);

			archive.Write(&bound);
			archive.Write(comp_pixels, bound);

			delete[] comp_pixels;
			delete[] pixels;
		}
	}

	void TextureManager::Load(Archive& archive, Renderer* renderer)
	{
		RemoveAll();

		UINT count;
		archive.Read(&count);

		for (UINT x = 0; x < count; ++x)
		{
			std::string name;
			archive.Read(&name);

			short width;
			archive.Read(&width);

			short height;
			archive.Read(&height);

			UINT size;
			archive.Read(&size);

			BYTE* comp_pixels = new BYTE[size];
			archive.Read(comp_pixels, size);

			ULONG bound = (width * 4) * height;
			BYTE* pixels = new BYTE[bound];

			uncompress(pixels, &bound, comp_pixels, (ULONG)size);

			auto newTexture = renderer->CreateTexture(pixels, width, height, ImageFormat::RGBA);

			if (!newTexture)
				throw std::exception("Failed to load texture.");

			std::shared_ptr<Texture> texture(newTexture);

			delete[] comp_pixels;
			delete[] pixels;

			Add(name, texture);
		}
	}
}
