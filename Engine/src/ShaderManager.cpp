#include "ShaderManager.h"
#include "Shader.h"
#include "Archive.h"
#include "Debug.h"
#include "Renderer.h"
#include "tinyxml.h"
#include "zlib.h"

namespace Sentinel
{
	ShaderManager::ShaderManager()
	{ }

	void ShaderManager::Save(Archive& archive)
	{
		UINT count = mData.size();
		archive.Write(&count);

		for (auto it : mData)
		{
			std::shared_ptr<Shader> shader = it.second;

			archive.Write(&it.first);

			char* source = (char*)shader->Source();
			ULONG  size = (ULONG)strlen(source);
			ULONG bound = compressBound(size);

			BYTE* comp_source = (BYTE*)malloc(bound);

			compress(comp_source, &bound, reinterpret_cast<const Bytef*>(source), size);

			archive.Write(&size);
			archive.Write(&bound);
			archive.Write(comp_source, bound);

			free(comp_source);
		}
	}

	void ShaderManager::Load(Archive& archive, Renderer* renderer)
	{
		RemoveAll();

		UINT count;
		archive.Read(&count);

		for (UINT x = 0; x < count; ++x)
		{
			std::string name;
			archive.Read(&name);

			ULONG size;
			archive.Read(&size);

			ULONG bound;
			archive.Read(&bound);

			char* comp_source = (char*)malloc(bound);
			archive.Read(comp_source, bound);

			char* source = (char*)malloc(size + 1);
			source[size] = 0;

			uncompress(reinterpret_cast<Bytef*>(source), &size, reinterpret_cast<Bytef*>(comp_source), bound);

			Debug::Log(STREAM("Compiling '" << name << "'..."));

			Add(name, std::shared_ptr<Shader>(renderer->CreateShaderFromMemory(source)));

			free(comp_source);
		}
	}
}
