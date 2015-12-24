#pragma once

#include "Sentinel.h"
#include "StringStream.h"

namespace Sentinel
{
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define TRACE __FILENAME__ << " (" << __LINE__ << "): "

	class SENTINEL_DLL Debug
	{
	public:

		static void Log(const std::string& text);
		static void Log(StringStream& stream);

		static void LogWarning(const std::string& text);
		static void LogWarning(StringStream& stream);

		static void ShowWarning(const std::string& msg, const std::string& title);
		static void ShowWarning(StringStream& msg, StringStream& title);

		static void LogError(const std::string& text);
		static void LogError(StringStream& stream);

		static void ShowError(const std::string& msg, const std::string& title);
		static void ShowError(StringStream& msg, StringStream& title);
	};
}
