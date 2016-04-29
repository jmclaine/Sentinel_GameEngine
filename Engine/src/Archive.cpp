#include "Archive.h"
#include "FloatCompressor.h"

#include <memory.h>
#include <exception>

namespace Sentinel
{
	Archive::Archive()
	{
		mFile = nullptr;
	}

	Archive::~Archive()
	{
		if (mFile != nullptr)
		{
			fclose(mFile);
			mFile = nullptr;
		}
	}

	bool Archive::Open(const char* filename, const char* mode)
	{
		mFile = fopen(filename, mode);

		return (mFile != nullptr);
	}

	void Archive::Close()
	{
		fclose(mFile);
	}

	////////////////////////////////////////////////////////////////////////////////

	void Archive::Read(std::string* data)
	{
		data->clear();

		char c = 0;
		Read(&c);

		while (c != 0)
		{
			data->push_back(c);
			Read(&c);
		}

		data->shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////////////////////

	void Archive::Write(const std::string* data)
	{
		Write(data->c_str(), (unsigned int)data->size());

		char end = 0;
		Write(&end);
	}

	/////////////////////////////////////////////////////////////

	unsigned int Archive::ToBuffer(const char *filename, char*& buf)
	{
		FILE* file = fopen(filename, "rb");

		if (!file)
			return 0;

		fseek(file, 0, SEEK_END);
		unsigned int length = (unsigned int)(ftell(file));
		rewind(file);

		buf = (char*)malloc(length + 1);
		fread(buf, sizeof(char), length, file);
		fclose(file);

		buf[length] = 0;

		return length;
	}
}
