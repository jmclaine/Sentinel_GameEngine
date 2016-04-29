#pragma once

#include <stdio.h>
#include <functional>

#include "Sentinel.h"
#include "FloatCompressor.h"

namespace Sentinel
{
	class SENTINEL_DLL Archive
	{
	public:
		FILE* mFile;

		////////////////////////

		Archive();
		~Archive();

		bool Open(const char* filename, const char* mode);
		void Close();

		////////////////////////

		template <typename T>
		void Read(T* data, UINT length = 1)
		{
			if (fread(data, sizeof(T), length, mFile) != length)
				throw std::exception("Failed to read from file");
		}

		template <typename T, typename U>
		void ReadConvert(T* data, UINT length = 1, std::function<T(const U&)> convert = [](const U& value) { return value; })
		{
			for (UINT x = 0; x < length; ++x)
			{
				U temp;
				if (fread(&temp, sizeof(U), 1, mFile) != length)
					throw std::exception("Failed to read from file");
				data[x] = convert(temp);
			}
		}

		void ReadConvert(float* data, UINT length = 1)
		{
			ReadConvert<float, int16_t>(data, length, [](const int16_t& value) { return FloatCompressor::Decompress(value); });
		}

		void Read(std::string* data);

		////////////////////////

		template <typename T>
		void Write(T* data, UINT length = 1)
		{
			if (fwrite(data, sizeof(T), length, mFile) != length)
				throw std::exception("Failed to write to file");
		}

		template <typename T, typename U>
		void WriteConvert(T* data, UINT length = 1, std::function<U(const T&)> convert = [](const T& value) { return value; })
		{
			for (unsigned int x = 0; x < length; ++x)
			{
				U temp = convert(data[x]);
				if (fwrite(&temp, sizeof(U), 1, mFile) != length)
					throw std::exception("Failed to write to file");
			}
		}

		void WriteConvert(float* data, UINT length = 1)
		{
			WriteConvert<float, int16_t>(data, length, [](const float& value) { return FloatCompressor::Compress(value); });
		}

		void Write(const std::string* data);

		////////////////////////

		// Returns size of the buffer
		//
		static unsigned int ToBuffer(const char *filename, char*& buf);
	};
}
